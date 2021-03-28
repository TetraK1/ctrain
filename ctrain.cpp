#include <iostream>
#include <vector>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class RailObject {
    public:
        const string id;
        RailObject(const string &id):
            id(id)
        {}

        virtual ~RailObject() {}

        friend ostream& operator<<(ostream& out, const RailObject& ro) {
            return out << "<RailObject: " << ro.id << ">";
        }

        static shared_ptr<RailObject> make(const json& j);
};

class SimpleNode: public RailObject {
    public:
        const string t1;
        const string t2;

        SimpleNode(const string& id, const string& t1, const string& t2):
            RailObject(id),
            t1(t1),
            t2(t2)
        {}
};

class EndNode: public RailObject {
    public:
        const string track;

        EndNode(const string &id, const string &track): 
            RailObject(id),
            track(track)
        {}

        EndNode(const json &j) : EndNode(j.at("id"), j.at("track")){}

        friend ostream& operator<<(ostream &out, const EndNode &en) {
            return out << "<EndNode: " << en.id << ">";
        }
};

class PointsNode: public RailObject {
    public:
        const string facing_track;
        const string forward_track;
        const string reverse_track;

        PointsNode(const string &id, const string &facing_track, const string &forward_track, const string &reverse_track): 
            RailObject(id),
            facing_track(facing_track),
            forward_track(forward_track),
            reverse_track(reverse_track)
        {}

        PointsNode(const json &j): PointsNode(j.at("id"), j.at("facing"), j.at("forward"), j.at("reverse")){}

        friend ostream& operator<<(ostream &out, const PointsNode &pn){
            return out << "<PointsNode: " << pn.id << ">";
        }
};

class Track: public RailObject {
    public:
        const string start_node;
        const string end_node;
        const double length;

        Track(const string &id, const string &start_node, const string &end_node, const double& length):
            RailObject(id),
            start_node(start_node),
            end_node(end_node),
            length(length)
        {}

        Track(const json &j): Track(j.at("id"), j.at("start_node"), j.at("end_node"), j.at("length")){}

        friend ostream& operator<<(ostream &out, const Track &t){
            return out << "<Track: " << t.id << ", length: " << t.length << ">";
        }
};

shared_ptr<RailObject> RailObject::make(const json& j) {
    const string& type = j["type"];
    if (type == "end_node") return make_shared<EndNode>(j);
    if (type == "points_node") return make_shared<PointsNode>(j);
    if (type == "track") return make_shared<Track>(j);
    return nullptr;
}

class Layout {
    public:
        map<string, shared_ptr<RailObject>> objects;

        Layout(const json &j){
            for (auto jo: j){
                shared_ptr<RailObject> ro(RailObject::make(jo));
                objects.insert(pair<string, shared_ptr<RailObject>>(ro->id, ro));
            }
        }

        friend ostream& operator<<(ostream &out, const Layout &l){
            out << "Layout:\n";
            for(auto ro: l.objects){
                out << "\t" << *(ro.second) << "\n";
            }
            return out;
        }
};

Layout load_layout_from_file(const string &fname){
    ifstream infile(fname);
    json j;
    infile >> j;
    return Layout(j);
}

int main(){
    unique_ptr<Layout> l;
    try {
        l = make_unique<Layout>(load_layout_from_file("layout.json"));
    } catch (exception& e) {
        cout << "Loading failed\n";
        cout << e.what() << '\n';
        throw;
    }
    cout << *l << "\n";
    for (auto o: l->objects){
        shared_ptr<Track> t = dynamic_pointer_cast<Track>(o.second);
        if (!t) continue;
        cout << *t << "\n";
    }
}