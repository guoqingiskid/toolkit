#ifndef STUDY5_H
#define STUDY5_H
#include <vector>
#include <initializer_list>

//visitor模式
//description：visitor设计模式主要用于一个visitor类家族需要"拜访"一个稳定的类继承体系A，A家族几乎不会再添加任何派生类以及接口
//A家族仅需要提供accept接口用于接受访问者拜访它们即可。拜访者仅能访问这些家族的公共区域。
class VisitorBase;
class Object
{
public:
	virtual void accept(VisitorBase *visitor) = 0;
};

class FirstObject : public Object
{
public:
	void accept(VisitorBase *visitor) override;

public:
	//FirstObject Operation
	void firstShow();
};

class SecondObject : public Object
{
public:
	void accept(VisitorBase *visitor) override;

public:
	//SecondObject Operation
	void SecondShow();
};

class VisitorBase
{
public:
	VisitorBase(const std::initializer_list<Object*> &args)
	{
		for (auto iter = args.begin(); iter != args.end(); ++iter)
			_objects.push_back(*iter);
	}

	virtual void start() = 0;
	virtual void visit(FirstObject *object) = 0;
	virtual void visit(SecondObject *object) = 0;

protected:
	std::vector<Object*> _objects;
};

class AVisitor : public VisitorBase
{
public:
	AVisitor(const std::initializer_list<Object*> &args);
	void start() override;
	virtual void visit(FirstObject *object);
	virtual void visit(SecondObject *object);
};

class BVisitor : public VisitorBase
{
public:
	BVisitor(const std::initializer_list<Object*> &args);
	void start() override;
	void visit(FirstObject *object) override;
	void visit(SecondObject *object) override;
};
#endif

//study5.cpp
AVisitor::AVisitor(const std::initializer_list<Object*> &args)
	:VisitorBase(args)
{

}

BVisitor::BVisitor(const std::initializer_list<Object*> &args)
	: VisitorBase(args)
{

}

void AVisitor::start()
{
	for (auto iter = _objects.begin(); iter != _objects.end(); ++iter)
	{
		if (*iter)
			(*iter)->accept(this);
	}
}

void AVisitor::visit(FirstObject * object)
{
	std::cout << "visitor A visit first object\n";
	object->firstShow();
}

void AVisitor::visit(SecondObject * object)
{
	std::cout << "visitor A visit second object\n";
	object->SecondShow();
}

void BVisitor::start()
{
	for (auto iter = _objects.rbegin(); iter != _objects.rend(); ++iter)
	{
		if (*iter)
			(*iter)->accept(this);
	}
}

void BVisitor::visit(FirstObject * object)
{
	std::cout << "visitor B visit first object\n";
	object->firstShow();
}

void BVisitor::visit(SecondObject * object)
{
	std::cout << "visitor B visit second object\n";
	object->SecondShow();
}


//Object implementation
void FirstObject::accept(VisitorBase * visitor)
{
	visitor->visitFirstObject(this);
}

void FirstObject::firstShow()
{
	std::cout << "first object show\n";
}

void SecondObject::SecondShow()
{
	std::cout << "second object show\n";
}

void SecondObject::accept(VisitorBase * visitor)
{
	visitor->visitSecondObject(this);
}

int main()
{
	Object *obj1 = new FirstObject;
	Object *obj2 = new SecondObject;
	AVisitor avisitor{ obj1, obj2 };
	BVisitor bvisitor{ obj1, obj2 };
	avisitor.start();
	bvisitor.start();
	delete obj1;
	delete obj2;
	return 1;
}
