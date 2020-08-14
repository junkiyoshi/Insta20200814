#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetColor(39);

	this->box2d.init();
	this->box2d.setGravity(0, 50);
	this->box2d.createBounds();
	this->box2d.setFPS(60);
	this->box2d.registerGrabbing();

	this->radius = 8;
}

//--------------------------------------------------------------
void ofApp::update() {

	if (ofGetFrameNum() % 8 == 0) {

		auto circle = make_shared<ofxBox2dCircle>();
		circle->setPhysics(0.5, 0.83, 0.1);
		circle->setup(this->box2d.getWorld(), ofRandom(10, ofGetWidth() - 10), 10, this->radius);
		this->circle_list.push_back(circle);

		vector<glm::vec2> log;
		this->log_list.push_back(log);
		this->life_list.push_back(255);
	}


	for (int i = 0; i < this->circle_list.size(); i++) {

		for (int j = i + 1; j < this->circle_list.size(); j++) {

			float distance = this->circle_list[i]->getPosition().distance(this->circle_list[j]->getPosition());
			if (distance < this->radius * 4) {

				this->circle_list[i]->addForce(this->circle_list[i]->getPosition() - this->circle_list[j]->getPosition(), ofMap(distance, this->radius, this->radius * 4, 5, 0.02));
				this->circle_list[j]->addForce(this->circle_list[j]->getPosition() - this->circle_list[i]->getPosition(), ofMap(distance, this->radius, this->radius * 4, 5, 0.02));
			}
		}
	}

	this->box2d.update();

	for (int i = this->circle_list.size() - 1; i > -1; i--) {

		this->log_list[i].push_back(glm::vec2(this->circle_list[i]->getPosition().x, this->circle_list[i]->getPosition().y));
		while (this->log_list[i].size() > 15) {

			this->log_list[i].erase(this->log_list[i].begin());
		}

		this->life_list[i] -= 1;
		if (this->life_list[i] < 0) {

			this->circle_list.erase(this->circle_list.begin() + i);
			this->log_list.erase(this->log_list.begin() + i);
			this->life_list.erase(this->life_list.begin() + i);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	for (int i = 0; i < this->circle_list.size(); i++) {

		if (this->log_list[i].size() < 2) continue;

		ofMesh mesh;
		vector<glm::vec3> right, left, frame;
		glm::vec3 last_location;
		float last_theta;

		for (int k = 0; k < this->log_list[i].size() - 1; k++) {

			auto location = glm::vec3(this->log_list[i][k], 0);
			auto next = glm::vec3(this->log_list[i][k + 1], 0);

			auto direction = next - location;
			auto theta = atan2(direction.y, direction.x);

			right.push_back(location + glm::vec3(ofMap(k, 0, this->log_list[i].size(), 0.1, 8) * cos(theta + PI * 0.5), ofMap(k, 0, this->log_list[i].size(), 0.1, 8) * sin(theta + PI * 0.5), 0));
			left.push_back(location + glm::vec3(ofMap(k, 0, this->log_list[i].size(), 0.1, 8) * cos(theta - PI * 0.5), ofMap(k, 0, this->log_list[i].size(), 0.1, 8) * sin(theta - PI * 0.5), 0));

			last_location = location;
			last_theta = theta;
		}

		for (int k = 0; k < right.size(); k++) {

			mesh.addVertex(left[k]);
			mesh.addVertex(right[k]);
		}

		for (int k = 0; k < mesh.getNumVertices() - 2; k += 2) {

			mesh.addIndex(k + 0); mesh.addIndex(k + 1); mesh.addIndex(k + 3);
			mesh.addIndex(k + 0); mesh.addIndex(k + 2); mesh.addIndex(k + 3);
		}

		mesh.addVertex(last_location);
		int index = mesh.getNumVertices();
		for (auto theta = last_theta - PI * 0.5; theta <= last_theta + PI * 0.5; theta += PI / 20) {

			mesh.addVertex(last_location + glm::vec3(8 * cos(theta), 8 * sin(theta), 0));
			frame.push_back(last_location + glm::vec3(8 * cos(theta), 8 * sin(theta), 0));
		}

		for (int k = index; k < mesh.getNumVertices() - 1; k++) {

			mesh.addIndex(index); mesh.addIndex(k + 0); mesh.addIndex(k + 1);
		}

		this->life_list[i] > 128 ? ofSetColor(39) : ofSetColor(39, ofMap(this->life_list[i], 0, 128, 0, 255));
		mesh.draw();
	}
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}