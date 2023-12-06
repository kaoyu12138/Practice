#include <bits/stdc++.h>
using namespace std;

template<typename T>
class Queue {
	public:
		Queue() {
			_front = _rear = new QueueItem();
		}

		~Queue() {
			QueueItem *cur = _front;
			while (cur != nullptr) {
				_front = _front -> next;
				delete cur;
				cur = _front;
			}
		}

		void push(const T &val) {
			QueueItem *item = new QueueItem(val);
			_rear -> _next = item;
			_rear = item;
		}

		void pop() {
			if (empty())
				return;

			QueueItem *first = _front->_next;
			_front->_next = first->next;

			if (_front->next == nullptr) {
				_rear = _front;
			}
			delete first;
		}

		T front() const {
			return _front->next->data;
		}

		bool empty()const {
			return _front == _rear;
		}
	private:
		//在QueueItem里实现重载的new和delete, 其他地方调用new QueueItem就会使用重载版本
		struct QueueItem {
			QueueItem(T data = T()): _data(data), _next(nullptr) {
			}

			static void *operator new (size_t size) {
				if (_itemPool == nullptr) {			 //如果itemPool指向nullptr，则申请空间
					_itemPool = (QueueItem *) new char[POOL_ITEM_SIZE * sizeof(QueueItem)];

					QueueItem *p = _itemPool;		  //将对象池中的对象连接起来
					for (; p < _itemPool + POOL_ITEM_SIZE - 1; ++ p) {
						p -> next = p + 1;
					}
					p -> next = nullptr;
				}

				QueueItem *p = _itemPool;
				_itemPool = _itemPool->next;
				return p;
			}

			static void operator delete (void *ptr) {
				QueueItem *p = (QueueItem *)ptr;
				p -> next = _itemPool;
				_itemPool = p;  					  //将节点归还给对象池
			}

			T _data;
			QueueItem *_next;
			static const int POOL_ITEM_SIZE = 100000; 
			static QueueItem *_itemPool;              
		};

		QueueItem *_front; //ָ指向头节点
		QueueItem *_rear;  //ָ指向尾节点
};

template<typename T>
typename Queue<T>::QueueItem *Queue<T>::QueueItem::_itemPool = nullptr;