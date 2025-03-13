#pragma once
#include "Stulu/Math/Math.h"

struct _MonoDomain;
typedef struct _MonoDomain MonoDomain;

struct _MonoClass;
typedef struct _MonoClass MonoClass;

struct _MonoAssembly;
typedef struct _MonoAssembly MonoAssembly;

struct _MonoImage;
typedef struct _MonoImage MonoImage;

struct _MonoMethod;
typedef struct _MonoMethod MonoMethod;

struct _MonoThread;
typedef struct _MonoThread MonoThread;

struct _MonoObject;
typedef struct _MonoObject MonoObject;

struct _MonoString;
typedef struct _MonoString MonoString;

struct MonoMethodDesc;
typedef struct MonoMethodDesc MonoMethodDesc;

struct _MonoReflectionType;
typedef struct _MonoReflectionType MonoReflectionType;

struct _MonoType;
typedef struct _MonoType MonoType;

struct _MonoClassField;
typedef struct _MonoClassField MonoClassField;

struct MonoVTable;
typedef struct MonoVTable MonoVTable;

struct _MonoArrayType;
typedef struct _MonoArrayType MonoArrayType;

struct _MonoArray;
typedef struct _MonoArray MonoArray;

struct _MonoCustomAttrEntry;
struct _MonoCustomAttrInfo;

#ifndef NO_EXPORT_MONO_WRAPPER
	#define ST_MONO_API STULU_API
#endif

namespace Stulu {
	namespace Mono {
		class ST_MONO_API Type;
		class ST_MONO_API ReflectionType;
		class ST_MONO_API Method;
		class ST_MONO_API Class;
		class ST_MONO_API String;
		class ST_MONO_API Object;
		class ST_MONO_API Domain;
		class ST_MONO_API ClassField;
		class ST_MONO_API Assembly;
		class ST_MONO_API MethodDesc;
		class ST_MONO_API MethodDesc;
		class ST_MONO_API Thread;
		class ST_MONO_API GCHandle;
		class ST_MONO_API CustomAttrInfo;
		class ST_MONO_API CustomAttrEntry;
		class ST_MONO_API Array;

		class ST_MONO_API Image {
		public:
			Image(MonoImage* ptr)
				: m_image(ptr) {};

			void Close();
			
			inline operator bool() const {
				return m_image != nullptr;
			}
			inline operator MonoImage*() {
				return m_image;
			}
			enum class Status {
				OK,
				ERROR_ERRNO,
				MISSING_ASSEMBLYREF,
				IMAGE_INVALID
			};
			static std::string GetStatusError(Status status);

			static Image Open(const std::string& fname, Status* status);
			static Image OpenFull(const std::string& fname, Status* status, bool refonly);
			static Image OpenFromData(char* data, uint32_t data_len, bool need_copy, Status* status);
			static Image OpenFromDataFull(char* data, uint32_t data_len, bool need_copy, Status* status, bool refonly);
		private:
			MonoImage* m_image;
		};

		class ST_MONO_API Assembly {
		public:
			Assembly(MonoAssembly* ptr)
				: m_assembly(ptr) {};

			Image GetImage() const;

			inline operator bool() const {
				return m_assembly != nullptr;
			}
			inline operator MonoAssembly* () {
				return m_assembly;
			}

			static Assembly LoadFromFull(Image image, const std::string& fname, Image::Status* status, bool refonly);
		private:
			MonoAssembly* m_assembly;
		};

		class ST_MONO_API Domain {
		public:
			Domain(MonoDomain* ptr)
				: m_domain(ptr) {};

			void Unload();
			bool Set(bool force);
			Assembly OpenAssembly(const std::string& name) const;

			inline operator bool() const {
				return m_domain != nullptr;
			}
			inline operator MonoDomain* () {
				return m_domain;
			}

			inline static Domain Get() { return GetDomain(); }
			static MonoDomain* GetDomain();

			inline static Domain CreateAppDomain(const std::string& name) { return Domain(name, ""); }
			inline static Domain CreateAppDomain(const std::string& name, const std::string& configFile) { return Domain(name, configFile); }
		private:
			//create_appdomain(char* friendly_name, char* configuration_file);
			Domain(const std::string& name, const std::string& configFile);

			MonoDomain* m_domain;
		};

		enum class TypeNameFormat {
			IL,
			REFLECTION,
			FULL_NAME,
			ASSEMBLY_QUALIFIED
		};
		class ST_MONO_API Type {
		public:
			Type(MonoType* ptr)
				: m_type(ptr) {};

			std::string GetName() const;
			std::string GetNameFull(TypeNameFormat format) const;

			int GetType() const;
			/* For MONO_TYPE_CLASS, VALUETYPE */
			Class GetClass() const;
			MonoArrayType* GetArrayType() const;
			/* For MONO_TYPE_PTR */
			Type GetPtrType() const;
			Class GetModifiers(bool* is_required, void** iter) const;

			bool IsStruct() const;
			bool IsVoid() const;
			bool IsPointer() const;
			bool IsRefrence() const;
			bool IsGenericParameter() const;

			inline operator bool() const {
				return m_type != nullptr;
			}
			inline operator MonoType* () {
				return m_type;
			}
		private:
			MonoType* m_type;
		};
		
		class ST_MONO_API ReflectionType {
		public:
			ReflectionType(MonoReflectionType* ptr)
				: m_type(ptr) {};

			Type GetType() const;

			inline operator bool() const {
				return m_type != nullptr;
			}
			inline operator MonoReflectionType* () {
				return m_type;
			}
		private:
			MonoReflectionType* m_type;
		};

		class ST_MONO_API Method {
		public:
			Method(MonoMethod* ptr)
				: m_method(ptr) {};

			const std::string GetFullName(bool signature)  const;

			inline operator bool() const {
				return m_method != nullptr;
			}
			inline operator MonoMethod* () {
				return m_method;
			}
		private:
			MonoMethod* m_method;
		};

		class ST_MONO_API Class {
		public:
			Class(MonoClass* ptr)
				: m_class(ptr) {};

			std::string GetName() const;
			std::string GetNamespace() const;

			bool IsEnum() const;
			uint32_t GetTypeToken() const;
			Class GetParent() const;
			
			ClassField GetField(uint32_t field_token) const;
			ClassField GetFieldFromName(const std::string& name) const;
			ClassField GetFields(void** iter) const;

			Method GetMethodFromName(const std::string& name, int param_count) const;

			inline bool operator ==(const Class& left) const {
				return this->m_class == left.m_class;
			}
			inline operator bool() const {
				return m_class != nullptr;
			}
			inline operator MonoClass*() {
				return m_class;
			}

			static Class FromName(Image image, const std::string& nameSpace, const std::string& name);
		private:
			static bool Compare(MonoClass* left, MonoClass* right);

			MonoClass* m_class;
		};


		class ST_MONO_API CustomAttrEntry {
		public:
			CustomAttrEntry(_MonoCustomAttrEntry* entry)
				: m_entry(entry) {}

			Method GetConstructor() const;
			size_t GetDataSize() const;
			void* GetData() const;

			inline operator bool() const {
				return m_entry != nullptr;
			}
			inline operator _MonoCustomAttrEntry* () {
				return m_entry;
			}
		private:
			_MonoCustomAttrEntry* m_entry;
		};

		class ST_MONO_API CustomAttrInfo {
		public:
			CustomAttrInfo(_MonoCustomAttrInfo* infos)
				: m_infos(infos){}

			static CustomAttrInfo FromField(Class clas, ClassField field);
			static CustomAttrInfo FromClass(Class clas);
			static CustomAttrInfo FromMethod(Method method);

			bool HasAttribute(Class attribute) const;
			Object GetAttribute(Class attribute) const;
			void Free();

			inline operator bool() const {
				return m_infos != nullptr;
			}
			inline operator _MonoCustomAttrInfo* () {
				return m_infos;
			}
		private:
			_MonoCustomAttrInfo* m_infos;
		};


		class ST_MONO_API String {
		public:
			String(MonoString* ptr)
				: m_string(ptr) {};

			std::string ToUtf8() const;
			inline operator bool() const {
				return m_string != nullptr;
			}
			inline operator MonoString* () {
				return m_string;
			}
			static String New(Domain main, const std::string& text);

		private:
			MonoString* m_string;
		};

		class ST_MONO_API Object {
		public:
			Object(MonoObject* ptr)
				: m_object(ptr) {};

			String ToString(MonoObject** exc = nullptr) const;
			Class GetClass() const;

			inline operator bool() const {
				return m_object != nullptr;
			}
			inline operator MonoObject* () {
				return m_object;
			}

			static Object New(Domain domain, Class klass);
		private:
			MonoObject* m_object;
		};

		class ST_MONO_API ClassField {
		public:
			ClassField(MonoClassField* ptr)
				: m_field(ptr) {};

			std::string GetName() const;
			Type GetType() const;
			Class GetParent() const;
			uint32_t GetFlags() const;
			uint32_t GetOffset() const;
			const char* GetData() const;
			inline void SetValue(Object obj, void* value) const { return s_SetValue(obj, m_field, value); }
			inline void SetValueStatic(MonoVTable* vt, void* value) const { return s_SetValueStatic(vt, m_field, value); }
			inline void GetValue(Object obj, void* value) const { return s_GetValue(obj, m_field, value); }
			inline void GetValueStatic(MonoVTable* vt, void* value) const { return s_GetValueStatic(vt, m_field, value); }
			inline Object GetValueObject(Domain domain, Object obj) const { return s_GetValueObject(domain, m_field, obj); }


			inline operator bool() const {
				return m_field != nullptr;
			}
			inline operator MonoClassField* () {
				return m_field;
			}
			inline bool operator ==(const ClassField& left) const {
				return this->m_field == left.m_field;
			}
		private:
			static void s_SetValue(Object obj, ClassField field, void* value);
			static void s_SetValueStatic(MonoVTable* vt, ClassField field, void* value);
			static void s_GetValue(Object obj, ClassField field, void* value);
			static void s_GetValueStatic(MonoVTable* vt, ClassField field, void* value);
			static Object s_GetValueObject(Domain domain, ClassField field, Object obj);

			MonoClassField* m_field;
		};

		class ST_MONO_API MethodDesc {
		public:
			MethodDesc(MonoMethodDesc* ptr)
				: m_methodDesc(ptr) {};

			void Free();
			bool Match(Method method) const;
			bool IsFull() const;
			bool FullMatch(Method method) const;
			Method SearchInClass(Class klass) const;
			Method SearchInImage(Image image) const;

			inline operator bool() const {
				return m_methodDesc != nullptr;
			}
			inline operator MonoMethodDesc* () {
				return m_methodDesc;
			}
			static MethodDesc New(const std::string& name, bool include_namespace);
			static MethodDesc FromMethod(Method method);
		private:
			MonoMethodDesc* m_methodDesc;
		};

		class ST_MONO_API Thread {
		public:
			Thread(MonoThread* ptr)
				: m_thread(ptr) {};

			inline operator bool() const {
				return m_thread != nullptr;
			}
			inline operator MonoThread* () {
				return m_thread;
			}

			static Thread GetCurrent();
			static Thread GetMain();
			static void SetMain(Thread thread);
		private:
			MonoThread* m_thread;
		};

		class ST_MONO_API GCHandle {
		public:
			GCHandle(uint32_t ptr)
				: m_handle(ptr) {};

			void Free();
			Object GetTarget();

			inline operator uint32_t () {
				return m_handle;
			}

			static GCHandle New(Object obj, bool pinned);
			static GCHandle NewWeakRef(Object obj, bool track_resurrection);
		private:
			uint32_t m_handle;
		};
		class ST_MONO_API Array {
		public:
			Array(MonoArray* ptr)
				: m_array(ptr) {};

			static Array New(Domain domain, Class clas, size_t size);

			void SetRef(size_t index, Object value);

			inline operator bool() const {
				return m_array != nullptr;
			}
			inline operator MonoArray* () {
				return m_array;
			}
		private:
			MonoArray* m_array;
		};

		namespace JIT {
			ST_MONO_API Domain Init(const std::string& file);
			ST_MONO_API Domain InitVersion(const std::string& root_domain_name, const std::string& runtime_version);
			ST_MONO_API int Exec(Domain domain, Assembly assembly, int argc, char* argv[]);
			ST_MONO_API void Cleanup(Domain domain);
			ST_MONO_API bool set_trace_options(const char* options);
			ST_MONO_API void set_signal_chaining(bool chain_signals);
			ST_MONO_API void set_crash_chaining(bool chain_signals);
			/**
			 * Allows control over our AOT (Ahead-of-time) compilation mode.
			 */
			enum class AotMode {
				/* Disables AOT mode */
				NONE,
				/* Enables normal AOT mode, equivalent to mono_jit_set_aot_only (false) */
				NORMAL,
				/* Enables hybrid AOT mode, JIT can still be used for wrappers */
				HYBRID,
				/* Enables full AOT mode, JIT is disabled and not allowed,
				 * equivalent to mono_jit_set_aot_only (true) */
				FULL,
				/* Same as full, but use only llvm compiled code */
				LLVMONLY,
				/* Uses Interpreter, JIT is disabled and not allowed,
				 * equivalent to "--full-aot --interpreter" */
				INTERP,
				/* Same as INTERP, but use only llvm compiled code */
				INTERP_LLVMONLY,
				/* Use only llvm compiled code, fall back to the interpeter */
				LLVMONLY_INTERP,
				/* Sentinel value used internally by the runtime. We use a large number to avoid clashing with some internal values. */
				LAST = 1000,
			};

			ST_MONO_API void SetAotMode(AotMode mode);

			/*
			 * Returns whether the runtime was invoked for the purpose of AOT-compiling an
			 * assembly, i.e. no managed code will run.
			 */
			ST_MONO_API bool AotCompiling();

			/* Allow embedders to decide wherther to actually obey breakpoint instructions
			 * in specific methods (works for both break IL instructions and Debugger.Break ()
			 * method calls).
			 */
			enum class BreakPolicy {
				/* the default is to always obey the breakpoint */
				ALWAYS,
				/* a nop is inserted instead of a breakpoint */
				NEVER,
				/* the breakpoint is executed only if the program has ben started under
				 * the debugger (that is if a debugger was attached at the time the method
				 * was compiled).
				 */
				ON_DBG
			};

			typedef BreakPolicy(*BreakPolicyFunc) (Method method);
			ST_MONO_API void SetBreakPolicy(BreakPolicyFunc policy_callback);
			ST_MONO_API void ParseOptions(int argc, char* argv[]);
			ST_MONO_API char* GetRuntimeBuildInfo();
			ST_MONO_API void SetUseLLVM(bool use_llvm);
			ST_MONO_API void AotRegisterModule(void** aot_info);
			ST_MONO_API Domain JitThreadAttach(Domain domain);
		}

		ST_MONO_API void SetDirs(const std::string& assembly_dir, const std::string& config_dir);
		ST_MONO_API Domain GetRootDomain();
		ST_MONO_API Class GetObjectClass();
		ST_MONO_API void AddInternallCall(const std::string& name, const void* method);
		ST_MONO_API Object RuntimeInvoke(Method method, void* obj, void** params, MonoObject** exc = NULL);
		ST_MONO_API void RuntimeObjectInit(Object object);
	}
}