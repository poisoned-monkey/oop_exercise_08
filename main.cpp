#include<iostream>
#include<memory>
#include<vector>
#include<thread>
#include<mutex>
#include<Windows.h>
#include<future>
#include"pentagon.h"
#include"trapeze.h"
#include"rhombus.h"
#include"figure.h"
#include"factory.h"


void print_to_file(std::vector<std::unique_ptr<figure>>& figures, std::string& filename) {
	for (int i = 0; i < figures.size(); ++i) {
		figures[i]->print(filename);
	}
}
void print_to_console(std::vector<std::unique_ptr<figure>>& figures) {
	for (int i = 0; i < figures.size(); ++i) {
		figures[i]->print(std::cout);
	}
}

void handle(std::vector<std::unique_ptr<figure>>& figures,int buffer_size, std::mutex& mtx) {
	int file_count = 0;
	while (true) {

		if (figures.size() == buffer_size) {
			mtx.lock();
			++file_count;
			std::string filename = "file_";
			filename += std::to_string(file_count) + ".txt";
			for (int i = 0; i < figures.size(); ++i) {
				/*
				figures[i]->print(std::cout);
				figures[i]->print(filename);
				//*/
				//*
				auto a = std::async(print_to_file, ref(figures), ref(filename) );
				auto b = std::async(print_to_console,ref(figures));
				//*/
			}
			figures.clear();
			mtx.unlock();
		}	
	}
}
int main() {
	std::vector<std::unique_ptr<figure>> figures;
	std::unique_ptr<factory> my_factory;
	std::mutex mtx;
	int buffer_size, menu;
	std::cin >> buffer_size;
	figures.reserve(buffer_size);
	std::thread handler(handle, ref(figures), buffer_size,ref(mtx));
	while (true) {
		mtx.lock();
		for (int i = 0; i < buffer_size; ++i) {
			std::cout << "1. Pentagon" << std::endl;
			std::cout << "2. Rhombus" << std::endl;
			std::cout << "3. Trapeze" << std::endl;
			std::cin >> menu;
			switch (menu) {
			case 1 :
				my_factory = std::make_unique<pentagon_factory>();
				figures.push_back(my_factory->build(std::cin));
				break;
			case 2 :
				my_factory = std::make_unique<rhombus_factory>();
				figures.push_back(my_factory->build(std::cin));
				break;
			case 3 :
				my_factory = std::make_unique<trapeze_factory>();
				figures.push_back(my_factory->build(std::cin));
				break;
			}
		}
		mtx.unlock();
		Sleep(50);
		mtx.lock();
		std::cout << "Continue? 'y' - Yes 'n' - No" << std::endl;
		char answer;
		std::cin >> answer;
		mtx.unlock();
		if (answer != 'y')
			break;
	}
	handler.detach();
	return 0;
}