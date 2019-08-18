#ifndef __TEST_DATA_H__
#define __TEST_DATA_H__

#include <algorithm>    // std::generate
#include <string>
#include <vector>       // std::vector
#include <cstdlib>      // std::rand, std::srand
#include <iostream>
#include <fstream>		// std::fstream
#include <sstream> 		// std::stringstream

template<typename T>
struct RandomNumber {
	float min_, max_;
	RandomNumber() {min_ = -1.0; max_ = 1.0; srand(time(0));}
	RandomNumber(T rmin, T rmax) {min_ = (float)rmin; max_ = (float)rmax; srand(time(0));}
	T operator()() {
		return (rand()*(max_ - min_)/RAND_MAX + min_);
	}
};

template<typename T>
struct SequenceNumber {
	T current_, step_;
	SequenceNumber() {current_ = 0; step_= 1;}
	SequenceNumber(T begin) {current_ = begin; step_ = 1;}
	SequenceNumber(T begin, T step) {current_ = begin; step_ = step;}
	T operator()() {
		T tmp = current_;
		current_ += step_;
		return tmp;
	}
};

template<typename T>
struct ConstNumber {
	T val_;
	ConstNumber() {val_ = 0;}
	ConstNumber(T v) {val_ = v;}
	T operator()() {return val_;}
};


template <typename T> class TestData;
template<typename T>
bool operator==(const TestData<T> &lhs, const TestData<T> &rhs) {
	if(lhs.count_ != rhs.count_) {
		return false;
	}
	for(int i=0;i<lhs.count_;i++) {
		if(fabs(lhs.data_[i] - rhs.data_[i]) > TestData<T>::tolerance_) {
			return false;
		}
	}
	return true;
}

template<typename T>
bool operator!=(const TestData<T> &lhs, const TestData<T> &rhs) {
	return (!(lhs == rhs));
}

template<typename T>
class TestData {
public:
	friend bool operator==<T>(const TestData<T> &lhs, const TestData<T> &rhs);
	friend bool operator!=<T>(const TestData<T> &lhs, const TestData<T> &rhs);

	TestData(){};
	TestData(int n, int c, int h, int w);
	TestData(int size);
	TestData(const std::vector < int > & shape);
	~TestData(){/*free(data_);*/};
	void reshape(int size);
	void reshape(int n, int c, int h, int w);
	void reshape(const std::vector < int > & shape);
	void generateSequence(T begin, T step);
	void generateRandom(T rmin, T rmax);
	void generateConst(T constVal);

	int count() const {return count_;}
	float tolerance() const {return tolerance_;}

	void show(std::string tag=" ", int interval=8);
	void writeToBuf(void *host);  // size is count_*sizeof(T)
	void writeToBuf(std::vector<T> &host);  // size is count_*sizeof(T)
	void readFromBuf(void *host); // size is count_*sizeof(T)
	void readFromBuf(const std::vector<T> &host); // size is count_*sizeof(T)
	int writeToDisk(std::string filename);
	int readFromDisk(std::string filename);

private:
	std::vector<T> data_;//nchw...
	std::vector<int> shape_;
	std::vector<int> shape_stride_;
	int count_;
	static float tolerance_;
};

template<typename T>
float TestData<T>::tolerance_=0;

template<typename T>
TestData<T>::TestData(int n, int c, int h, int w) {
	std::vector<int> shape(4);
	shape[0] = n;
	shape[1] = c;
	shape[2] = h;
	shape[3] = w;
	reshape(shape);
}

template<typename T>
TestData<T>::TestData(int size) {
	std::vector<int> shape(1);
	shape[0] = size;
	reshape(shape);
}

template<typename T>
TestData<T>::TestData(const std::vector<int>& shape) {
	reshape(shape);
}

template<typename T>
void TestData<T>::reshape(int size) {
	std::vector<int> shape(1);
	shape[0] = size;
	reshape(shape);
}

template<typename T>
void TestData<T>::reshape(int n, int c, int h, int w) {
	std::vector<int> shape(4);
	shape[0] = n;
	shape[1] = c;
	shape[2] = h;
	shape[3] = w;
	reshape(shape);
}

template<typename T>
void TestData<T>::reshape(const std::vector<int>& shape) {
	shape_.resize(shape.size());
	shape_stride_.resize(shape.size());

	count_ = 1;
	for (unsigned int i = 0; i < shape.size(); i++) {
		count_ *= shape[i];
		shape_[i] = shape[i];
	}

	//init shape_stride_[]
	shape_stride_[shape.size()-1] = 1;
	for(int i = shape.size() - 2; i >= 0; i-- ) {
		shape_stride_[i] = shape_stride_[i+1] * shape_[i+1];
	}
	//data_ = (T*)malloc(sizeof(T)*count_);
	data_.resize(count_);
}

template<typename T>
inline void TestData<T>::generateSequence(T begin, T step) {
	generate (data_.begin(), data_.end(), SequenceNumber<T>(begin, step));
}

template<typename T>
inline void TestData<T>::generateRandom(T rmin, T rmax) {
	generate (data_.begin(), data_.end(), RandomNumber<T>(rmin, rmax));
}

template<typename T>
inline void TestData<T>::generateConst(T constVal) {
	generate (data_.begin(), data_.end(), ConstNumber<T>(constVal));
}

template<typename T>
void TestData<T>::readFromBuf(void *host) {
	T *pxBufHost = (T *)host;
	for(int i = 0; i < count_; i++) {
		data_[i] = pxBufHost[i];
	}
}

template<typename T>
void TestData<T>::writeToBuf(void *host) {
	T *pxBufHost = (T *)host;
	for(int i = 0; i < count_; i++) {
		pxBufHost[i] = data_[i];
	}
}

template<typename T>
void TestData<T>::readFromBuf(const std::vector<T> &host) {
	reshape(host.size());
	for(int i = 0; i < count_; i++) {
		data_[i] = host[i];
	}
}

template<typename T>
void TestData<T>::writeToBuf(std::vector<T> &host) {
	host.resize(count_);
	for(int i = 0; i < count_; i++) {
		host[i] = data_[i];
	}
}

template<typename T>
int TestData<T>::readFromDisk(std::string filename)
{
	std::fstream file(filename, std::ios::in);
	if(file.bad()) {
		std::cout << "open file " << filename << " fail." << std::endl;
		return -1;
	}
	std::string header1;
	std::string tag1 = "size: ";
	getline(file, header1);
	if(header1.find(tag1) == header1.npos) {
		std::cout << "cannot read size from target file." << std::endl;
		return -1;
	}

	std::string sSize = header1.substr(header1.find(tag1) + tag1.size());
	std::stringstream ssSize(sSize);
	int size;
	if(!(ssSize >> size)) {
		std::cout << "error read size." << std::endl;
		return -1;
	} else if (size <= 0) {
		//std::cout<< "size is " << size << std::endl;
		return -1;
	} else {
		//std::cout<< "size is " << size << std::endl;
		std::vector<T> tmpData(size);
		int cnt = 0;
		while(file >> tmpData[cnt]) {	//read sth, blank divided.
			cnt++;
		}
		if(cnt != size) {
			std::cout << "size("<<size<<") not match with actual data count("<<cnt<<")." << std::endl;
			return -1;
		} else {
			std::cout << "total read " << size << " data." << std::endl;
			reshape(size);
			tmpData.swap(data_);
		}
	}
	return count_;
}

template<typename T>
int TestData<T>::writeToDisk(std::string filename)
{
	std::fstream file(filename, std::ios::out);
	if(file.bad()) {
		std::cout << "open file " << filename << " fail." << std::endl;
		return -1;
	}
	file << "size: " << count_ << std::endl;
	for(int i = 0; i < count_; i++)
		file << data_[i] <<" ";
	file.close();
	return 0;
}

template<typename T>
void TestData<T>::show(std::string tag, int interval) {
	std::vector<int> lastpos(shape_.size(), -1);

	std::cout<<tag<<":(float)";
	for(int i = 0; i < count_; i++) {
		lastpos.resize(shape_.size(),0);
		for(unsigned int j = 0; j < shape_.size(); j++) {
			int cur = (i/shape_stride_[j])%shape_[j];
			//print format
			if(cur != lastpos[j]) {
				if(j == shape_.size()-1) {
					std::cout<<" ";
				} else if(j == shape_.size()-2) {
					std::cout<<std::endl;
				} else {
					std::cout<<std::endl<<std::endl;
					for(unsigned int t = 0; t < shape_.size(); t++) {
						if(t < j)
							std::cout<<lastpos[t];
						else if(t == j)
							std::cout<<cur;
						else
							std::cout<<"*";
					}
				}
			}
			lastpos[j] = cur;
		}
		std::cout.width(interval);
		std::cout<<data_[i];
	}
	std::cout<<std::endl;
}

#endif

