/************************************************************************
FormatString.cpp

作者：魏炜 pokifox@qq.com
************************************************************************/

#include <string>
#include <memory>
#include <vector>

#define REMOVE_POINTER(U)	typename std::remove_pointer<U>::type

#define IS_INTEGER(U)		std::is_integral<U>::value
#define IS_ENUM(U)			std::is_enum<U>::value
#define IS_STRING(U)		std::is_same<U, std::string>::value
#define IS_SAME(U1, U2)		std::is_same<U1, U2>::value
#define IS_CLASS(U)			std::is_class<U>::value
#define IS_SHAREDPOINTER(U)	class_shared<U>::value
#define IS_CONTAINER(U)		class_container<U>::value
#define IS_POINTER(U)		std::is_pointer<U>::value
#define HAS_FORMATSTRING(U)	class_formatstring<U>::value

#define CASE_NUMBER		template<typename T, typename std::enable_if<!IS_CLASS(T) && IS_INTEGER(T), T>::type* = nullptr>
#define CASE_ENUM		template<typename T, typename std::enable_if<IS_ENUM(T), T>::type* = nullptr>
#define CASE_STRING		template<typename T, typename std::enable_if<IS_CLASS(T) && IS_STRING(T), T>::type* = nullptr>
#define CASE_CLASS		template<typename T, typename std::enable_if<IS_CLASS(T) && !IS_STRING(T) && !IS_SHAREDPOINTER(T) && !IS_CONTAINER(T) && HAS_FORMATSTRING(T), T>::type* = nullptr>
#define CASE_SHAREDPTR	template<typename T, typename std::enable_if<IS_CLASS(T) && IS_SHAREDPOINTER(T), T>::type* = nullptr>
#define CASE_CONTAINER	template<typename T, typename std::enable_if<IS_CLASS(T) && !IS_STRING(T) && IS_CONTAINER(T), T>::type* = nullptr>
#define CASE_PCHAR		template<typename T, typename std::enable_if<IS_POINTER(T) && IS_INTEGER(REMOVE_POINTER(T)), T>::type* = nullptr>
#define CASE_PCLASS		template<typename T, typename std::enable_if<IS_POINTER(T) && IS_CLASS(REMOVE_POINTER(T)) && !IS_STRING(REMOVE_POINTER(T)) && !IS_SHAREDPOINTER(REMOVE_POINTER(T)) && !IS_CONTAINER(REMOVE_POINTER(T)), T>::type* = nullptr>
#define CASE_UNKNOWN	template<typename T, typename std::enable_if<!(false																																\
						/*CASE_NUMBER*/		|| (!IS_CLASS(T) && IS_INTEGER(T))																																\
						/*CASE_ENUM*/		|| (IS_ENUM(T))																																					\
						/*CASE_STRING*/		|| (IS_CLASS(T) && IS_STRING(T))																																\
						/*CASE_CLASS*/		|| (IS_CLASS(T) && !IS_STRING(T) && !IS_SHAREDPOINTER(T) && !IS_CONTAINER(T) && HAS_FORMATSTRING(T))															\
						/*CASE_SHAREDPTR*/	|| (IS_CLASS(T) && IS_SHAREDPOINTER(T))																															\
						/*CASE_CONTAINER*/	|| (IS_CLASS(T) && !IS_STRING(T) && IS_CONTAINER(T))																											\
						/*CASE_PCHAR*/		|| (IS_POINTER(T) && IS_INTEGER(REMOVE_POINTER(T)))																												\
						/*CASE_PCLASS*/		|| (IS_POINTER(T) && IS_CLASS(REMOVE_POINTER(T)) && !IS_STRING(REMOVE_POINTER(T)) && !IS_SHAREDPOINTER(REMOVE_POINTER(T)) && !IS_CONTAINER(REMOVE_POINTER(T)))	\
						), T>::type* = nullptr>

template<typename U> struct class_shared {
	template<typename T, typename element_type = typename T::element_type> static constexpr bool checkfunction(T*) { return true; };
	static constexpr bool checkfunction(...) { return false; };
	static constexpr bool value = checkfunction(static_cast<U*>(nullptr));
};

template<typename U> struct class_container {
	template<typename T, typename value_type = typename T::value_type> static constexpr bool checkfunction(T*) { return true; }
	static constexpr bool checkfunction(...) { return false; }
	static constexpr bool value = checkfunction(static_cast<U*>(nullptr));
};

template <typename U> struct class_formatstring {
	template <typename T, std::string(T::*)() = &T::formatstring> static constexpr bool checkfunction(T*) { return true; };
	static constexpr bool checkfunction(...) { return false; };
	static constexpr bool value = checkfunction(static_cast<U*>(0));
};

CASE_NUMBER		std::string tostring(T& t) { return std::to_string(t); }
CASE_ENUM		std::string tostring(T& t) { return std::to_string((long)t); }
CASE_STRING		std::string tostring(T& t) { return t; }
CASE_CLASS		std::string tostring(T& t) { return t.formatstring(); }
CASE_SHAREDPTR	std::string tostring(T& t) { return tostring(*t); }
CASE_CONTAINER	std::string tostring(T& t) { std::string r = ""; for (auto& v : t) r += tostring(v) + std::string(","); return r.length() > 0?r.substr(0, r.length() - 1):""; }
CASE_PCHAR		std::string tostring(T& t) { return std::string(t); }
CASE_PCLASS		std::string tostring(T& t) { return tostring(*t); }
//CASE_UNKNOWN	std::string tostring(T& t) { return "unknown"; }

std::string __formatstring(int ncount, const char* format)
{
	return std::string(format);
}

template <typename T, typename... Args> std::string __formatstring(int ncount, const char* format, T value, Args... args) {
	std::string countstr = std::string("{") + std::to_string(ncount) + "}";
	std::string formatstr = std::string(format);
	std::string valuestr = tostring(value);
	size_t position = 0;
	do {
		position = formatstr.find(countstr, position);
		if (position == formatstr.npos) break;
		formatstr = formatstr.replace(position, countstr.length(), valuestr);
	} while (true);
	return __formatstring(ncount + 1, formatstr.c_str(), args...);
};

template <typename... Args> std::string formatstring(const char* format, Args... args) {
	return __formatstring(1, format, args...);
}

/*********************************************************************************************************************************/
/*********************************************************************************************************************************/
/*********************************************************************************************************************************/

enum class ENUM_A {
	ENUM_A_VALUE0 = 1,
};

class CLASS_A {
public:
	std::string formatstring() { return std::string("CLASS_A::formatstring."); }
};

class CLASS_B {
};

int main()
{
	int v1 = 12345;
	ENUM_A v2 = ENUM_A::ENUM_A_VALUE0;
	std::string v3 = std::string("v3");
	CLASS_A v4;
	std::shared_ptr<CLASS_A> v5 = std::make_shared<CLASS_A>();
	std::vector<CLASS_A> v6; v6.push_back(CLASS_A()); v6.push_back(CLASS_A()); v6.push_back(CLASS_A());
	const char* v7 = "value7";
	CLASS_A* v8 = new CLASS_A();

	std::string result = formatstring(	""			\
										"v1:{1}\n"	\
										"v2:{2}\n"	\
										"v3:{3}\n"	\
										"v4:{4}\n"	\
										"v5:{5}\n"	\
										"v6:{6}\n"	\
										"v7:{7}\n"	\
										"v8:{8}\n"	\
										, v1, v2, v3, v4, v5, v6, v7, v8);
	printf("%s\n", result.c_str());

//	CASE_UNKNOWN	std::string tostring(T& t) { return "unknown"; }
//	comment above will cause:
//	CLASS_B v9; std::string errorstring = formatstring("v9:{1}", v9);	// compile error. class CLASS_B has no member named 'formatstring'
//	printf("%s\n", errorstring.c_str());

	return 0;
}

// g++ -std=c++17 FormatString.cpp

/*
v1:12345
v2:1
v3:v3
v4:CLASS_A::formatstring.
v5:CLASS_A::formatstring.
v6:CLASS_A::formatstring.,CLASS_A::formatstring.,CLASS_A::formatstring.
v7:value7
v8:CLASS_A::formatstring.
//v9:unknown
*/