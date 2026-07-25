#[flutter_rust_bridge::frb(sync)]
pub fn greet(name: String) -> String {
    format!("Hello, {name}!")
}

#[flutter_rust_bridge::frb(init)]
pub fn init_app() {
    flutter_rust_bridge::setup_default_user_utils();

    #[cfg(target_os = "android")]
    ensure_android_context();
}

#[cfg(target_os = "android")]
fn ensure_android_context() {
    if std::panic::catch_unwind(ndk_context::android_context).is_ok() {
        log::info!("nano: ndk_context already initialized (by JNI_OnLoad)");
    } else {
        log::warn!("nano: ndk_context still not initialized after JNI_OnLoad");
    }
}

/// JNI_OnLoad is called by the ART runtime when System.loadLibrary() loads our .so.
/// We capture the JavaVM pointer here and initialize ndk_context,
/// so that midir can find it later.
#[cfg(target_os = "android")]
#[no_mangle]
pub extern "system" fn JNI_OnLoad(
    vm: *mut std::ffi::c_void,
    _reserved: *mut std::ffi::c_void,
) -> i32 {
    log::info!("nano: JNI_OnLoad called, initializing ndk_context");
    unsafe {
        ndk_context::initialize_android_context(vm, std::ptr::null_mut());
    }
    0x00010006 // JNI_VERSION_1_6
}
