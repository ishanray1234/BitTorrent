// This is a C++ version of the Go P2P Torrent download logic based on the provided client abstraction
// Assumes existence of analogous C++ classes for: Client, Peer, Bitfield, Message, etc.

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <atomic>
#include <chrono>
#include <unordered_map>
// #include <openssl/sha.h>
#include "../client/client.h"
#include "../peers/peer.h"
#include "../bitfield/bitfield.h"
#include "../message/message.h"

#define MAX_BLOCK_SIZE 16384
#define MAX_BACKLOG 5

struct PieceWork {
    int index;
    std::array<unsigned char, 20> hash;
    int length;
};

struct PieceResult {
    int index;
    std::vector<uint8_t> buf;
};

struct PieceProgress {
    int index;
    Client* client;
    std::vector<uint8_t> buf;
    int downloaded = 0;
    int requested = 0;
    int backlog = 0;

    bool readMessage() {
        auto msg = client->Read();
        if (!msg) return false;

        switch (msg->ID) {
            case MsgUnchoke:
                client->Choked = false;
                break;
            case MsgChoke:
                client->Choked = true;
                break;
            case MsgHave:
                client->Bitfield.SetPiece(msg->PieceIndex);
                break;
            case MsgPiece:
                int n;
                if (!ParsePiece(index, buf, *msg, n)) return false;
                downloaded += n;
                backlog--;
                break;
        }
        return true;
    }
};

bool checkIntegrity(PieceWork* pw, std::vector<uint8_t>& buf) {
    // uint8_t hash[20];
    // SHA1(buf.data(), buf.size(), hash);
    // return std::equal(std::begin(hash), std::end(hash), pw->hash.begin());
    return true;
}

std::vector<uint8_t> attemptDownloadPiece(Client* c, PieceWork* pw) {
    PieceProgress state = { pw->index, c, std::vector<uint8_t>(pw->length) };
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(30);

    while (state.downloaded < pw->length) {
        if (!state.client->Choked) {
            while (state.backlog < MAX_BACKLOG && state.requested < pw->length) {
                int blockSize = std::min(MAX_BLOCK_SIZE, pw->length - state.requested);
                if (!c->SendRequest(pw->index, state.requested, blockSize)) return {};
                state.backlog++;
                state.requested += blockSize;
            }
        }

        if (!state.readMessage()) return {};
    }
    return state.buf;
}

class Torrent {
public:
    std::vector<Peer> Peers;
    string PeerID;
    string InfoHash;
    vector<string> PieceHashes;
    int PieceLength;
    int Length;
    std::string Name;

    std::pair<int, int> calculateBoundsForPiece(int index) {
        int begin = index * PieceLength;
        int end = std::min(begin + PieceLength, Length);
        return { begin, end };
    }

    int calculatePieceSize(int index) {
        auto [begin, end] = calculateBoundsForPiece(index);
        return end - begin;
    }

    void startDownloadWorker(Peer peer, std::queue<PieceWork*>& workQueue, std::mutex& qMutex, 
                              std::vector<PieceResult>& results, std::mutex& resMutex, std::atomic<int>& donePieces) {
        Client* c = Client::New(peer, PeerID, InfoHash);
        if (!c) return;

        c->SendUnchoke();
        c->SendInterested();

        while (true) {
            PieceWork* pw = nullptr;
            {
                std::unique_lock<std::mutex> lock(qMutex);
                if (workQueue.empty()) break;
                pw = workQueue.front();
                workQueue.pop();
            }

            if (!c->Bitfield.HasPiece(pw->index)) {
                std::lock_guard<std::mutex> lock(qMutex);
                workQueue.push(pw);
                continue;
            }

            std::vector<uint8_t> buf = attemptDownloadPiece(c, pw);
            if (buf.empty() || !checkIntegrity(pw, buf)) {
                std::lock_guard<std::mutex> lock(qMutex);
                workQueue.push(pw);
                continue;
            }

            c->SendHave(pw->index);
            {
                std::lock_guard<std::mutex> lock(resMutex);
                results.push_back({ pw->index, buf });
                donePieces++;
                float percent = 100.0f * donePieces / PieceHashes.size();
                std::cout << "(" << percent << "%) Downloaded piece #" << pw->index << "\n";
            }
        }
        delete c;
    }

    std::vector<uint8_t> Download() {
        std::queue<PieceWork*> workQueue;
        for (size_t i = 0; i < PieceHashes.size(); i++) {
            workQueue.push(new PieceWork{ static_cast<int>(i), PieceHashes[i], calculatePieceSize(i) });
        }

        std::vector<PieceResult> results;
        std::mutex qMutex, resMutex;
        std::atomic<int> donePieces = 0;

        std::vector<std::thread> workers;
        for (Peer& peer : Peers) {
            workers.emplace_back(&Torrent::startDownloadWorker, this, peer, std::ref(workQueue), 
                                  std::ref(qMutex), std::ref(results), std::ref(resMutex), std::ref(donePieces));
        }
        for (auto& w : workers) w.join();

        std::vector<uint8_t> fileBuf(Length);
        for (const auto& res : results) {
            auto [begin, end] = calculateBoundsForPiece(res.index);
            std::copy(res.buf.begin(), res.buf.end(), fileBuf.begin() + begin);
        }
        return fileBuf;
    }
};
