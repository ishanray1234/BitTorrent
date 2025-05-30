#include<iostream>
#include "../peers/peer.h"

using namespace std;

class bencodeTrackerResp{
    public:
        int Interval; // Interval in seconds for the next request
        string Peers; // List of peers in the response

        // Constructor to initialize bencodeTrackerResp with metadata
        bencodeTrackerResp(int interval, const string& peersList): Interval(interval), Peers(peersList) {}
        // Function to display tracker response information
        void display() const {
            cout << "Interval: " << Interval << " seconds" << endl;cout << "Peers: " << Peers << endl;}
};

