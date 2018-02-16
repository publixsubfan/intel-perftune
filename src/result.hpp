#pragma once
#include <utility>

namespace perftune::util
{
	template<typename T, typename E>
	struct Result {
	public:
		Result(T expected)
			:_val(expected)
			,_isResult(true) { }

		Result(E error)
			:_val(error)
			,_isResult(false) {
		}

		Result(Result<T,E>& copy) {
			this->_val = copy._val;
			this->_isResult = copy._isResult;
		}

		Result(Result<T,E>&& move)
			:_isResult(std::move(move._isResult)) {
			if (_isResult) {
				this->_val._res = std::move(move._val._res);
			} else {
				this->_val._err = std::move(move._val._err);
			}
		}

		Result<T,E>& operator = (Result& lhs) {
			if (this != &lhs) {
				this->_val = lhs._val;
				this->_isResult = lhs._isResult;
			}
			return *this;
		}

		Result<T,E>& operator = (T expected) {
			this->_val._res = expected;
			this->_isResult = true;
			return *this;
		}

		Result<T,E>& operator = (E error) {
			this->val._err = error;
			this->_isResult = false;
			return *this;
		}

		~Result() {
		}

		operator bool() const {
			return _isResult;
		}

		T* operator ->() {
			return &(_val._res);
		}

		T operator *() {
			return _val._res;
		}

		T getResult() const {
			return _val._res;
		}

		E getError() const {
			return _val._err;
		}
	private:
		union _un_int{
			_un_int() {}
			_un_int(T res) : _res(res) {}
			_un_int(E err) : _err(err) {}
			~_un_int() {}
			T _res;
			E _err;
		} _val;
		bool _isResult;
	};
}