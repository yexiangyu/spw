use libffi::high::Closure1;
use log::*;
use spw::{self, AcqToBuf};
use std::thread::sleep;
use std::time::Duration;

pub fn callback_main(frm: spw::Frame) {
    info!("callback!!!!");
}

fn main() -> spw::Result<()> {
    std::env::set_var("RUST_LOG", "info");
    env_logger::init();
    // info!("start spw");
    let loc = spw::Location::new("Xtium-CL_MX4_1", 2);
    // info!("new location");
    // let mut config_file =
    //     "C://Program Files//Teledyne DALSA//Sapera//CamFiles//User//b_FullRGB_Default_Default.ccf"
    //         .to_string();
    let acq = spw::Acq::new(
        loc,
        "C://Program Files//Teledyne DALSA//Sapera//CamFiles//User//b_FullRGB_Default_Default.ccf",
    );
    info!("new acq");
    acq.create()?;
    info!("create acq");
    let buf = spw::Buffer::new(2, &acq);
    info!("new buf");
    buf.create()?;
    info!("create buf");
    let ctx = spw::Context::new();
    info!("new ctx");
    let callback = |frm: *mut spw_sys::Frame| {
        println!("fuck-1");
        let frm = unsafe { frm.as_ref().unwrap() };
        println!("fuck0");
        let frm = spw::Frame::from(*frm);
        println!("fuck1");
        callback_main(frm);
    };
    let callback = Closure1::new(&callback);
    // let ptr = callback.code_ptr();
    let ptr = Some(unsafe { std::mem::transmute(*callback.code_ptr()) });
    // info!("{:?}", ptr);
    // let pro = spw::Processing::new(&buf, Some(cb), &ctx);
    let pro = spw::Processing::new(&buf, ptr, &ctx);
    info!("new pro");
    pro.create()?;
    info!("create pro");
    ctx.attach_processing(&pro);
    info!("attach");
    let atb = AcqToBuf::new(&acq, &buf, &ctx);
    info!("atb");
    atb.create()?;
    info!("atb create");
    atb.grab()?;
    info!("atb grab");
    sleep(Duration::from_secs(10));
    info!("sleep done");
    // loop {
    //     if ctx.get_grab_frame_count() > 10 {
    //         break;
    //     }
    // }
    Ok(())
}
