#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<array>
#include "../peers/peer.h"
#include "../torrentfile/torrentfile.h"
#include "../bencode/torrent_parser.h"

using namespace std;

class bencodeTrackerResp{
    public:
        int Interval; // Interval in seconds for the next request
        string Peers; // List of peers in the response

        // Constructor to initialize bencodeTrackerResp with metadata
        // bencodeTrackerResp(int interval, const string& peersList): Interval(interval), Peers(peersList) {}
        // Function to display tracker response information
        void display() const {
            cout << "Interval: " << Interval << " seconds" << endl;cout << "Peers: " << Peers << endl;}
};

string fetchTrackerResponse(string url) {
    // This function should implement the logic to make an HTTP GET request to the tracker URL
    // and return the response as a string.
    // For demonstration purposes, let's assume it returns a dummy response.
    return "d8:intervali900e5:peers46:192.168.1.1:6881,10.0.0.2:51413,127.0.0.1:8080e";
}

// Function to percent-encode a byte array
string percentEncode(const unsigned char* data, size_t length) {
    ostringstream encoded;
    for (size_t i = 0; i < length; ++i) {
        encoded << '%' << hex << uppercase << setw(2) << setfill('0') << static_cast<int>(data[i]);
    }
    return encoded.str();
}

string buildTrackerURL(TorrentFile* torrent_file, string PeerID, uint16_t port) {
    string base = torrent_file->announce;
    
    map<string, string> params;
    params["info_hash"] = percentEncode(torrent_file->infoHash.data(), torrent_file->infoHash.size());
    params["peer_id"] = percentEncode(reinterpret_cast<const unsigned char*>(PeerID.data()), PeerID.size());
    params["port"] = to_string(port);
    params["uploaded"] = "0"; // Initial uploaded bytes
    params["downloaded"] = "0"; // Initial downloaded bytes
    params["compact"] = "1"; // Use compact peer format
    params["left"] = to_string(torrent_file->Length); // Total bytes left to download

    // Build the query string
    ostringstream query;
    bool first = true;
    for (const auto& [key, value] : params) {
        if (!first) query << "&";
        query << key << "=" << value;
        first = false;
    }

    return base + "?" + query.str();
}

vector<Peer> requestPeers(TorrentFile* torrent_file, string PeerID, uint16_t port) {
    string url = buildTrackerURL(torrent_file, PeerID, port);

    // Here you would typically use a library like libcurl to make the HTTP request
    // For demonstration, let's assume we have a function that fetches the response
    string response = fetchTrackerResponse(url);
    cout << "Tracker URL: " << url << endl;
    cout<< "Tracker response: " << response <<endl;
    size_t pos = 0;
    auto dictionary = decode_bencoded_value(response, pos);

    if(!dictionary.contains("interval") || !dictionary.contains("peers")) {
        throw runtime_error("Invalid tracker response: missing interval or peers field");
    }
    bencodeTrackerResp trackerResponse;
    trackerResponse.Interval = dictionary["interval"].get<int>();
    trackerResponse.Peers = dictionary["peers"].get<string>();

    cout<< trackerResponse.Interval << endl;
    cout<< trackerResponse.Peers <<"x"<< endl;

    if (trackerResponse.Peers.empty()) {
        throw runtime_error("Peers field is empty in tracker response");
    }
    vector<Peer> peers = Unmarshal(trackerResponse.Peers);

    cout << "Tracker Interval: " << trackerResponse.Interval << " seconds" << endl;
    cout << "Number of peers: " << peers.size() << endl;
    for (const auto& peer : peers) {
        peer.display();
    }
    return peers;
}
