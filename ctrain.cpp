#include <iostream>
#include <vector>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class SimpleNode {
    string id;
    string t1;
    string t2;
};

class EndNode {
    string id;
    string t;

    public:
    EndNode(const string &id, const string &t):
        id{id},
        t{t}
    {}

    EndNode(const json &j) : EndNode(j["id"], j["track"]){}

    string toString() const {return "<EndNode: " + id + ">";}
    friend ostream& operator<<(ostream &out, const EndNode &en){return out << en.toString();}
};

class PointsNode {
    string id;
    string facing_track;
    string forward_track;
    string reverse_track;

    public:
    PointsNode(const string &id, const string &facing_track, const string &forward_track, const string &reverse_track):
        id(id),
        facing_track(facing_track),
        forward_track(forward_track),
        reverse_track(reverse_track)
    {}

    PointsNode(const json &j): PointsNode(j["id"], j["facing"], j["forward"], j["reverse"]){}

    string toString() const{return "<PointsNode: " + id + ">";}
    friend ostream& operator<<(ostream &out, const PointsNode &pn){return out << pn.toString();}
};

class Track {
    string id;
    string start_node;
    string end_node;

    public:
    Track(const string &id, const string &start_node, const string &end_node):
        id(id),
        start_node(start_node),
        end_node(end_node)
    {}

    Track(const json &j): Track(j["id"], j["start_node"], j["end_node"]){}

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

Layout load_layout(const string &json_layout){
    vector<EndNode> end_nodes;
    vector<PointsNode> points_nodes;
    vector<Track> tracks;

    json j = json::parse(json_layout);

    for (auto en: j["end_nodes"]){end_nodes.push_back(EndNode(en));}
    for (auto pn: j["points_nodes"]){points_nodes.push_back(PointsNode(pn));}
    for (auto t: j["tracks"]){tracks.push_back(Track(t));}

    return Layout{end_nodes, points_nodes, tracks};
}

Layout load_layout_from_file(const string &fname){
    ifstream infile { fname };
    string file_contents { istreambuf_iterator<char>(infile), istreambuf_iterator<char>() };
    return load_layout(file_contents);
}

int main(){
    Layout l = load_layout_from_file("layout.json");
    cout << l << '\n';
}