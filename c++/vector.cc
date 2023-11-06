#include <bits/stdc++.h>
using namespace std;

template<typename T>
class Allocator {
		T *allocate(size_t size) {
			return (T *)malloc(sizeof(T) * size);
		}
		void deallocate(void *p) {
			free(p);
		}
		void construct(T *p, const T &val) {
			new (p) T(val); 
		}
		void destory(T *p) {
			p->~T();
		}
};

template<typename T, typename Alloc = Allocator<T>>
class vector {
	public:
		vector(int size = 10) {
			_first = _allocator.allocate(size);
			_last = _first;
			_end = _first + size;
		}

		~vector() {
			for (T *p = _first; p != _last; ++ p) {
				_allocator.destory(p); 
			}
			_allocator.deallocate(_first);
			_first = _last = _end;
		}

		vector(const vector<T> &rhs) {
			int size = rhs._end - rhs._first;
			_first = _allocator.allocate(size);
			int len = rhs._last - rhs._first;
			for (int i = 0; i < len; ++ i) {
				_allocator.construct(_first + i, rhs._first[i]);
			}
			_last = _first + len;
			_end = _first + size;
		}

		vector<T> &operator=(const vector<T> &rhs) {
			if (this == &rhs)
				return *this;

			for (T *p = _first; p != _last; ++ p) {
				_allocator.destory(p);
			}
			_allocator.deallocate(_first);

			int size = rhs._end - rhs._first;
			_first = _allocator.allocate(size);
			int len = rhs._last - rhs._first;
			for (int i = 0; i < len; ++ i) {
				_allocator.construct(_first + i, rhs._first[i]);
			}
			_last = _first + len;
			_end = _first + size;
		}

		void push_back(const T &val) {
			if (full())
				expand();
			_allocator.construct(_last, val);
			_last ++;
		}

		void pop_back() {
			if (empty())
				return;
			_last --;
			_allocator.destory(_last);
		}

		T back()const {
			return *(_last - 1);
		}
		bool full() {
			return _last == _end;
		}
		bool empty() {
			return _first == _last;
		}
		int size()const {
			return _last - _first;
		}
        T& opeartor [](int index){
            if (index < 0 || index >= size()){
                throw "OutOfRange"
            }
            return _first[index];
        }

        //实现vector的迭代器类
        class iterator{
            public:
                iterator(T* ptr) : _ptr(ptr){}
                bool operator!=(const iterator &it)const{
                    return _ptr != it._ptr;
                }
                T& operator*(){
                    return *_ptr;
                }
                void operator++(){
                    _ptr++;
                }   //前置++不产生临时量
            private:
                T *_ptr;
        };
        iterator begin() { return iterator(_first);}
        iterator end() { return iterator(_last); }
 	private:
		T *_first;  //指向数组的起始位置
		T *_last;   //指向数组有效元素的后继位置
		T *_end;    //指向数组空间的后继位置
		Alloc _allocator;   //定义容器的空间配置器对象

		void expand() {
			int size = _end - _first;
			T *ptmp = _allocator.allocate(2 * size);
			for (int i = 0; i < size; ++ i) {
				_allocator.construct(ptmp + i, _first[i]);
			}

			for (T *p = _first; p != _last; ++ p) {
				_allocator.destory(p);
			}
			_allocator.deallocate(_first);

			_first = ptmp;
			_last = _first + size;
			_end = _first + 2 * size;
		}
};
