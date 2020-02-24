#ifndef STUDY4_H
#define STUDY4_H
#include <map>
#include <exception>

//缺省的错误处理，当传入的id不在factory中注册时，要么程序抛出异常(这里将抛出异常视为缺省错误处理)，要么返回nullptr
template<class BaseProduct, class IdentifierType>
class DefaultFactoryError
{
public:
	class FactoryException : public std::exception
	{
	public:
		FactoryException(const IdentifierType &id)
			: _id(id)
		{

		}

		const char* what()
		{
			return "Unknown type pass to GenericObjectFactory";
		}

	private:
		IdentifierType _id;
	};

protected:
	BaseProduct* UnknownProduct(const IdentifierType &id)
	{
		throw FactoryException(id);
	}
};

//一个泛型对象工厂
template< class BaseProduct, class IdentifierType,
	typename ProductCreator = BaseProduct*(*)(),
	template<class, typename>class FactoryErrorPolicy = DefaultFactoryError>
class GenericObjectFactory : public FactoryErrorPolicy<BaseProduct, IdentifierType>
{
public:
	void registerProduct(const IdentifierType &id, const ProductCreator &creator)
	{
		_productCreators.insert(std::make_pair(id, creator));
	}

	void unregisterProduct(const IdentifierType &id)
	{
		auto iter = _productCreators.find(id);
		if(iter != _productCreators.end())
			_productCreators.erase(iter);
	}

	template<typename... Args>
	ProductCreator* createProduct(const IdentifierType &id, Args... &&args)
	{
		auto iter = _productCreators.find(id);
		if(iter != _productCreators.end())
			return iter->second(std::forward<Args>(args)...);
		else
			return UnknownProduct(id);
	}

private:
	std::map<IdentifierType, ProductCreator>	_productCreators;
};

#endif