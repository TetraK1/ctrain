#include <iostream>
#include <vector>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class SimpleNode {
    public:
        const string id;
        const string t1;
        const string t2;
};

class EndNode {
    public:
        const string id;
        const string t;

    EndNode(const string &id, const string &t):
        id{id},
        t{t}
    {}

    EndNode(const json &j) : EndNode(j.at("id"), j.at("track")){}

    string toString() const {return "<EndNode: " + id + ">";}
    friend ostream& operator<<(ostream &out, const EndNode &en){return out << en.toString();}
};

class PointsNode {
    public:
        const string id;
        const string facing_track;
        const string forward_track;
        const string reverse_track;

        PointsNode(const string &id, const string &facing_track, const string &forward_track, const string &reverse_track):
            id(id),
            facing_track(facing_track),
            forward_track(forward_track),
            reverse_track(reverse_track)
        {}

        PointsNode(const json &j): PointsNode(j.at("id"), j.at("facing"), j.at("forward"), j.at("reverse")){}

        string toString() const{return "<PointsNode: " + id + ">";}
        friend ostream& operator<<(ostream &out, const PointsNode &pn){return out << pn.toString();}
};

class Track {
    public:
        const string id;
        const string start_node;
        const string end_node;

        Track(const string &id, const string &start_node, const string &end_node):
            id(id),
            start_node(start_node),
            end_node(end_node)
        {}

        Track(const json &j): Track(j.at("id"), j.at("start_node"), j.at("end_node")){}

        string toString() const{return "<Track: " + id + ">";}
        friend ostream& operator<<(ostream &out, const Track &l){return out << l.toString();}
};

class Layout {
    public:
        vector<EndNode> end_nodes;
        vector<PointsNode> points_nodes;
        vector<Track> tracks;

        Layout(vector<EndNode> end_nodes, vector<PointsNode> points_nodes, vector<Track> tracks):
        end_nodes(end_nodes),
        points_nodes(points_nodes),
        tracks(tracks)
        {}

        Layout(const json &j){
            for (auto en: j.at("end_nodes")) {end_nodes.push_back(EndNode(en));}
            for (auto pn: j.at("points_nodes")) {points_nodes.push_back(PointsNode(pn));}
            for (auto t: j.at("tracks")) {tracks.push_back(Track(t));}
        }

        Track get_track(const string& t1_id, const string& t2_id){
            for (auto t: tracks){
                if (t1_id == t.start_node && t2_id == t.end_node) return t;
                if (t2_id == t.start_node && t1_id == t.end_node) return t;
            }
            throw std::exception("No matching track");
        }

        friend ostream& operator<<(ostream &out, const Layout &l){
            out << "Layout:\n";
            out << "    End nodes:\n";
            for (auto en: l.end_nodes) {out << "        " << en << '\n';}
            out << "    Points nodes:\n";
            for (auto pn: l.points_nodes) {out << "        " << pn << '\n';}
            out << "    Tracks:\n";
            for (auto t: l.tracks) {out << "        " << t << '\n';}
            return out;
        }
};

Layout load_layout_from_file(const string &fname){
    ifstream infile { fname };
    string file_contents { istreambuf_iterator<char>(infile), istreambuf_iterator<char>() };
    json j = json::parse(file_contents);
    return Layout(j);
}

int main(){
    std::unique_ptr<Layout> l(nullptr);

    try {
        l = std::make_unique<Layout>(load_layout_from_file("layout.json"));
    }
    catch (...) {
        cout << "Error loading layout\n";
        throw;
    }
    cout << *l;
    cout << l->get_track("PN001", "EN001");
}