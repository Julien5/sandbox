#[flutter_rust_bridge::frb(sync)] // Synchronous mode for simplicity of the demo
pub fn greet(name: String) -> String {
    format!("Hello, {name}!")
}

#[flutter_rust_bridge::frb(init)]
pub fn init_app() {
    // Default utilities - feel free to customize
    flutter_rust_bridge::setup_default_user_utils();

    #[cfg(target_os = "android")]
    init_android_context();
}

#[cfg(target_os = "android")]
fn init_android_context() {
    use std::ffi::c_void;

    if std::panic::catch_unwind(ndk_context::android_context).is_ok() {
        return;
    }

    let jvm_ptr = get_java_vm();
    let Some(jvm_ptr) = jvm_ptr else {
        log::warn!("Could not get JavaVM — MIDI will not work on Android");
        return;
    };

    unsafe {
        ndk_context::initialize_android_context(jvm_ptr as *mut c_void, std::ptr::null_mut());
    }
}

// Resolve JNI_GetCreatedJavaVMs via dlsym at runtime (not link-time),
// since the symbol is not exported by any shared library linked to our .so.
#[cfg(target_os = "android")]
fn get_java_vm() -> Option<*mut std::ffi::c_void> {
    use std::ffi::CString;

    type JniGetCreatedJavaVMs = unsafe extern "C" fn(
        *mut *mut std::ffi::c_void,
        i32,
        *mut i32,
    ) -> i32;

    extern "C" {
        fn dlsym(handle: *mut std::ffi::c_void, symbol: *const u8) -> *mut std::ffi::c_void;
    }

    // RTLD_DEFAULT on Linux/Android = ((void*)0)
    const RTLD_DEFAULT: *mut std::ffi::c_void = std::ptr::null_mut();

    let symbol = CString::new("JNI_GetCreatedJavaVMs").ok()?;
    let func_ptr = unsafe { dlsym(RTLD_DEFAULT, symbol.as_ptr() as *const u8) };
    if func_ptr.is_null() {
        return None;
    }

    let func: JniGetCreatedJavaVMs = unsafe { std::mem::transmute(func_ptr) };

    let mut vm: *mut std::ffi::c_void = std::ptr::null_mut();
    let mut n_vms: i32 = 0;
    let result = unsafe { func(&mut vm, 1, &mut n_vms) };
    if result == 0 && n_vms > 0 && !vm.is_null() {
        Some(vm)
    } else {
        None
    }
}
