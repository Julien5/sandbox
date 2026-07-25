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

    // Check if ndk_context is already initialized (via android-activity/ndk-glue)
    if std::panic::catch_unwind(ndk_context::android_context).is_ok() {
        return;
    }

    // Get the JavaVM via the JNI invocation API
    let jvm_ptr = get_java_vm();
    let Some(jvm_ptr) = jvm_ptr else {
        log::warn!("Could not get JavaVM — MIDI will not work on Android");
        return;
    };

    // Initialize ndk_context with a null context_jobject.
    // jni-min-helper will detect null and fall back to ActivityThread.
    unsafe {
        ndk_context::initialize_android_context(jvm_ptr as *mut c_void, std::ptr::null_mut());
    }
}

#[cfg(target_os = "android")]
type JniInt = i32;

#[cfg(target_os = "android")]
enum JniJavaVm {}

#[cfg(target_os = "android")]
extern "C" {
    fn JNI_GetCreatedJavaVMs(
        vm_buf: *mut *mut JniJavaVm,
        buf_len: JniInt,
        n_vms: *mut JniInt,
    ) -> JniInt;
}

#[cfg(target_os = "android")]
fn get_java_vm() -> Option<*mut JniJavaVm> {
    unsafe {
        let mut vm: *mut JniJavaVm = std::ptr::null_mut();
        let mut n_vms: JniInt = 0;
        let result = JNI_GetCreatedJavaVMs(&mut vm, 1, &mut n_vms);
        if result == 0 && n_vms > 0 && !vm.is_null() {
            Some(vm)
        } else {
            None
        }
    }
}
