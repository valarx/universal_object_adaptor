#include <string>
#include <iostream>
#include <functional>
#include <algorithm>
#include <vector>
#include <type_traits>

struct A
{
    int field = 5;
    int getter() const { return field; }
};

template<typename TClass, typename TReturnType>
struct UniversalAdaptor
{
  UniversalAdaptor(std::function<TReturnType(const TClass&)>&& aFunctor) : mFunctor{std::move(aFunctor)} {}
  std::function<TReturnType(const TClass&)> mFunctor;
  bool operator()(const TClass& lhs, const TClass& rhs) const
  {
    return mFunctor(lhs) < mFunctor(rhs);
  }
};

template<typename TClass, typename TFieldType>
using TIsMemberFunction = typename std::enable_if<std::is_member_function_pointer<TFieldType TClass::*>::value, int>::type;

template<typename TClass, typename TFieldType>
using TIsMemberObject = typename std::enable_if<std::is_member_object_pointer<TFieldType TClass::*>::value, int>::type;

template<typename TClass, typename TFieldType,
         TIsMemberObject<TClass, TFieldType> = 1>
UniversalAdaptor<TClass, TFieldType> make_adaptor(TFieldType TClass::* aMember) {
    return UniversalAdaptor<TClass, TFieldType>{aMember};
}

template<typename TClass, typename TFieldType,
         TIsMemberFunction<TClass, TFieldType> = 1,
         typename TReturnType = typename std::result_of<TFieldType TClass::*(TClass)>::type>
UniversalAdaptor<TClass, TReturnType> make_adaptor(TFieldType TClass::* aMember) {
    return UniversalAdaptor<TClass, TReturnType>{aMember};
}

template<typename TClass, typename TReturnType>
UniversalAdaptor<TClass, TReturnType> make_adaptor(TReturnType (*aFunctor)(const TClass&))
{
  return UniversalAdaptor<TClass, TReturnType>{aFunctor};
}

int times_ten(const A& a) 
{
  return a.field * 10;
}

int main()
{
    std::vector<A> as = {{6},{5},{4},{3},{2},{1},{0}};
    std::sort(as.begin(), as.end(), make_adaptor(&A::field));
    std::sort(as.begin(), as.end(), make_adaptor(&A::getter));
    std::sort(as.begin(), as.end(), make_adaptor(times_ten));
    for (const auto a : as)
    {
        std::cout << a.field << std::endl;
    }
}
