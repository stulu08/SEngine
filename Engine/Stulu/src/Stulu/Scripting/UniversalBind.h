#pragma once
#include <functional>
#include <memory>
#include <variant>

#include "Managed/Mono.h"
#include "Managed/ScriptAssembly.h"

namespace Stulu {
	class UniversalCallBase {
	public:
		virtual ~UniversalCallBase() = default;

		// args is a pointer to an argument struct
		// ret is a pointer to memory where the return value should be stored
		virtual void Call(void* args) = 0;
		virtual bool IsValid() const = 0;
	};


	class NativeUniversalCall : public UniversalCallBase {
	public:
		using NativeFnWithRet = std::function<void(void*)>;

		NativeUniversalCall() = default;
		explicit NativeUniversalCall(NativeFnWithRet fn) : m_func(fn) {}
		explicit NativeUniversalCall(void(*fn)(void*)) : m_func(fn) {}

		void Call(void* args) override {
			if (m_func) m_func(args);
		}

		bool IsValid() const override {
			return static_cast<bool>(m_func);
		}

	private:
		NativeFnWithRet m_func;
	};


	class ManagedUniversalCall : public UniversalCallBase {
	public:
		ManagedUniversalCall() = default;
		ManagedUniversalCall(MonoObject* instance, MonoMethod* method)
			: m_instance(instance), m_method(method) {}

		void Call(void* args) override {
			ScriptAssembly::InvokeMethod(m_method, m_instance, &args);
		}

		bool IsValid() const override {
			return m_method != nullptr;
		}

	private:
		MonoObject* m_instance = nullptr;
		MonoMethod* m_method = nullptr;
	};

	class UniversalBind {
	public:
		UniversalBind() = default;

		template<typename T>
		UniversalBind(std::unique_ptr<T> impl) : m_impl(std::move(impl)) {}

		static UniversalBind CreateNative(std::function<void(void*, void*)> fn) {
			return UniversalBind(std::make_unique<NativeUniversalCall>(std::move(fn)));
		}

		static UniversalBind CreateNative(void(*fn)(void*, void*)) {
			return UniversalBind(std::make_unique<NativeUniversalCall>(fn));
		}

		static UniversalBind CreateManaged(MonoObject* instance, MonoMethod* method) {
			return UniversalBind(std::make_unique<ManagedUniversalCall>(instance, method));
		}

		void Call(void* args) const {
			if (m_impl) m_impl->Call(args);
		}

		bool IsValid() const {
			return m_impl && m_impl->IsValid();
		}

	private:
		std::unique_ptr<UniversalCallBase> m_impl;
	};


}