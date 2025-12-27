#![no_main]
#![no_std]

use core::time::Duration;
use log::{error, info};
use uefi::boot::{self, SearchType};
use uefi::data_types::PoolString;
use uefi::prelude::*;
use uefi::proto::device_path::text::{AllowShortcuts, DevicePathToText, DisplayOnly};
use uefi::proto::loaded_image::LoadedImage;
use uefi::{Identify, Result};

#[entry]
fn main() -> Status {
    if let Err(error) = uefi::helpers::init() {
        return error.status();
    }

    let image_path = match get_image_path() {
        Ok(path) => path,
        Err(error) => return error.status(),
    };

    info!("UEFI application started. Image path: {}", &*image_path);

    info!("Stalling for 10 seconds...");
    boot::stall(Duration::from_secs(10));

    info!("Exiting UEFI application.");
    Status::SUCCESS
}

fn get_image_path() -> Result<PoolString> {
    let loaded_image =
        boot::open_protocol_exclusive::<LoadedImage>(boot::image_handle()).map_err(|error| {
            error!("Failed to open LoadedImage protocol: {:?}", error);
            error
        })?;

    let device_path_handles = boot::locate_handle_buffer(SearchType::ByProtocol(
        &DevicePathToText::GUID,
    ))
    .map_err(|error| {
        info!("Failed to locate DevicePathToText handle: {:?}", error);
        error
    })?;

    let device_path_to_text_handle = device_path_handles.first().ok_or_else(|| {
        error!("DevicePathToText protocol is not available");
        Status::NOT_FOUND
    })?;

    let device_path_to_text = boot::open_protocol_exclusive::<DevicePathToText>(
        *device_path_to_text_handle,
    )
    .map_err(|error| {
        error!("Failed to open DevicePathToText protocol: {:?}", error);
        error
    })?;

    let image_device_path = loaded_image.file_path().ok_or_else(|| {
        error!("Image file path is not set");
        Status::NOT_FOUND
    })?;

    let image_device_path_text = device_path_to_text
        .convert_device_path_to_text(image_device_path, DisplayOnly(true), AllowShortcuts(false))
        .map_err(|error| {
            error!("Failed to convert device path to text: {:?}", error);
            error
        })?;

    Ok(image_device_path_text)
}
