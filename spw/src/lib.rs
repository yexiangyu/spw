use spw_sys;
use std::ffi::CString;
use std::mem::ManuallyDrop;
use std::slice::from_raw_parts;

#[derive(Debug, thiserror::Error)]
pub enum Error {
    #[error("Nullptr")]
    Nullptr,
    #[error("AcqCreateErr")]
    AcqCreateErr,
    #[error("BufferCreateErr")]
    BufferCreateErr,
    #[error("ProcessingCreateErr")]
    ProcessingCreateErr,
    #[error("AcqToBufCreateErr")]
    AcqToBufCreateErr,
    #[error("AcqToBufGrabErr")]
    AcqToBufGrabErr,
    #[error("AcqToBufFreezeErr")]
    AcqToBufFreezeErr,
}

pub type Result<T> = std::result::Result<T, Error>;

#[derive(Debug, Clone)]
pub struct Frame {
    pub w: u64,
    pub h: u64,
    pub c: u64,
    pub timestamp: u64,
    pub frame_id: u64,
    pub data: Vec<u8>,
}

impl From<spw_sys::Frame> for Frame {
    fn from(frm: spw_sys::Frame) -> Self {
        let spw_sys::Frame {
            w,
            h,
            c,
            timestamp,
            frame_id,
            data,
        } = frm;
        // let data =
        //     unsafe { Vec::<u8>::from_raw_parts(data, (w * h * c) as usize, (w * h * c) as usize) };
        let data = unsafe { from_raw_parts(data, (w * h * c) as usize) }.to_vec();
        Self {
            w,
            h,
            c,
            timestamp,
            frame_id,
            data,
        }
    }
}

impl From<Frame> for spw_sys::Frame {
    fn from(frm: Frame) -> Self {
        let Frame {
            w,
            h,
            c,
            timestamp,
            frame_id,
            data,
        } = frm;
        let mut data = ManuallyDrop::new(data);
        let data = data.as_mut_ptr() as *mut std::os::raw::c_uchar;
        spw_sys::Frame {
            w,
            h,
            c,
            timestamp,
            frame_id,
            data,
        }
    }
}

macro_rules! impl_drop {
    ($cls: ty, $drop: ident) => {
        impl Drop for $cls {
            fn drop(&mut self) {
                unsafe { spw_sys::$drop(self.inner) };
            }
        }
    };
}

macro_rules! impl_fn {
    ($cls: ty, $fn_name: ident, $fn_impl: ident, $err: ident) => {
        impl $cls {
            pub fn $fn_name(&self) -> Result<()> {
                match unsafe { spw_sys::$fn_impl(self.inner) } <= 0 {
                    true => Err(Error::$err),
                    false => Ok(()),
                }
            }
        }
    };
}

// macro_rules! impl_create {
//     ($cls: ty, $create: ident, $err: ident) => {
//         impl $cls {
//             pub fn create(&self) -> Result<()> {
//                 match unsafe { spw_sys::$create(self.inner) } <= 0 {
//                     true => Err(Error::$err),
//                     false => Ok(()),
//                 }
//             }
//         }
//     };
// }

#[derive(Debug)]
pub struct Location {
    pub inner: spw_sys::Location,
}

impl Location {
    pub fn new(server_name: &str, device_id: i32) -> Self {
        let server_name = CString::new(server_name).expect("cstring?");
        Self {
            inner: unsafe { spw_sys::location_new(server_name.as_ptr(), device_id) },
        }
    }
}

impl_drop!(Location, location_free);

pub struct Acq {
    pub inner: spw_sys::Acq,
}

impl Acq {
    pub fn new(location: Location, conf: &str) -> Self {
        let Location { inner: location } = location;
        let conf = CString::new(conf).expect("cstring?");
        let ret = Self {
            inner: unsafe { spw_sys::acq_new(location, conf.as_ptr()) },
        };
        ret
    }
}

impl_drop!(Acq, acq_free);
impl_fn!(Acq, create, acq_create, AcqCreateErr);

pub struct Buffer {
    pub inner: spw_sys::Buffer,
}

impl Buffer {
    pub fn new(count: i32, acq: &Acq) -> Self {
        Self {
            inner: unsafe { spw_sys::buffer_new(count, acq.inner) },
        }
    }
}

impl_drop!(Buffer, buffer_free);
impl_fn!(Buffer, create, buffer_create, BufferCreateErr);

pub struct Context {
    pub inner: spw_sys::Context,
}

impl Context {
    pub fn new() -> Self {
        Self {
            inner: unsafe { spw_sys::context_new() },
        }
    }

    pub fn get_grab_frame_count(&self) -> i32 {
        unsafe { spw_sys::context_grab_frame_count(self.inner) }
    }

    pub fn get_proc_frame_count(&self) -> i32 {
        unsafe { spw_sys::context_proc_frame_count(self.inner) }
    }

    pub fn counter_reset(&self) {
        unsafe { spw_sys::context_counter_reset(self.inner) }
    }

    pub fn attach_processing(&self, proc: &Processing) {
        unsafe { spw_sys::context_attach_processing(self.inner, proc.inner) }
    }
}

impl_drop!(Context, context_free);


pub struct Processing {
    pub inner: spw_sys::Processing,
    // pub callback: spw_sys::FrameCallback,
}


pub type FrameCallback = spw_sys::FrameCallback;

impl Processing {
    pub fn new(buf: &Buffer, callback: FrameCallback, ctx: &Context) -> Self {
        Self {
            inner: unsafe { spw_sys::processing_new(buf.inner, callback, ctx.inner) },
        }
    }

    pub fn bind(&self) {}
}

impl_drop!(Processing, processing_free);
impl_fn!(Processing, create, processing_create, ProcessingCreateErr);

pub struct AcqToBuf {
    pub inner: spw_sys::AcqToBuf,
}

impl AcqToBuf {
    pub fn new(acq: &Acq, buf: &Buffer, ctx: &Context) -> Self {
        Self {
            inner: unsafe { spw_sys::acq_to_buffer_new(acq.inner, buf.inner, ctx.inner) },
        }
    }
}

impl_drop!(AcqToBuf, acq_to_buffer_free);
impl_fn!(AcqToBuf, create, acq_to_buffer_create, AcqToBufCreateErr);
impl_fn!(AcqToBuf, grab, acq_to_buffer_grab, AcqToBufGrabErr);
impl_fn!(AcqToBuf, freeze, acq_to_buffer_grab, AcqToBufFreezeErr);
