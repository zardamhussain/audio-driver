/*++

Copyright (c) Microsoft Corporation All Rights Reserved

Module Name:

    loopbackwavtable.h

Abstract:-

    Declaration of wave miniport tables for the loopback.

--*/

#ifndef _SIMPLEAUDIOSAMPLE_LOOPBACKWAVTABLE_H_
#define _SIMPLEAUDIOSAMPLE_LOOPBACKWAVTABLE_H_

//
// Loopback range.
//
#define LOOPBACK_MAX_CHANNELS            2       // Max channels overall
#define LOOPBACK_BITS_PER_SAMPLE_PCM     16      // 16 Bits Per Sample
#define LOOPBACK_SAMPLE_RATE             48000   // Sample rate

//
// Max # of pin instances.
//
#define LOOPBACK_MAX_STREAMS              1

//=============================================================================
static
KSDATAFORMAT_WAVEFORMATEXTENSIBLE LoopbackPinSupportedDeviceFormats[] =
{
    // 48 KHz 16-bit 2 channels
    {
        {
            sizeof(KSDATAFORMAT_WAVEFORMATEXTENSIBLE),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        {
            {
                WAVE_FORMAT_EXTENSIBLE,
                2,
                48000,
                192000, // 48000 * 2 channels * (16 bits / 8 bits per byte)
                4,
                16,
                sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)
            },
            16,
            KSAUDIO_SPEAKER_STEREO,
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM)
        }
    }
};

//
// Supported modes (only on streaming pins).
//
static
MODE_AND_DEFAULT_FORMAT LoopbackPinSupportedDeviceModes[] =
{
    {
        STATIC_AUDIO_SIGNALPROCESSINGMODE_DEFAULT,
        &LoopbackPinSupportedDeviceFormats[0].DataFormat
    }
};

//
// The entries here must follow the same order as the filter's pin
// descriptor array.
//
static
PIN_DEVICE_FORMATS_AND_MODES LoopbackPinDeviceFormatsAndModes[] =
{
    {
        BridgePin, // Input from speaker (bridge pin)
        NULL,
        0,
        NULL,
        0
    },
    {
        SystemCapturePin, // Output to system (capture pin)
        LoopbackPinSupportedDeviceFormats,
        SIZEOF_ARRAY(LoopbackPinSupportedDeviceFormats),
        LoopbackPinSupportedDeviceModes,
        SIZEOF_ARRAY(LoopbackPinSupportedDeviceModes)
    }
};

//=============================================================================
// Data ranges
//
static
KSDATARANGE_AUDIO LoopbackPinDataRangesStream[] =
{
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            KSDATARANGE_ATTRIBUTES,         // An attributes list follows this data range
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        LOOPBACK_MAX_CHANNELS,
        LOOPBACK_BITS_PER_SAMPLE_PCM,
        LOOPBACK_BITS_PER_SAMPLE_PCM,
        LOOPBACK_SAMPLE_RATE,
        LOOPBACK_SAMPLE_RATE
    }
};

static
PKSDATARANGE LoopbackPinDataRangePointersStream[] =
{
    PKSDATARANGE(&LoopbackPinDataRangesStream[0]),
    PKSDATARANGE(&PinDataRangeAttributeList),
};

//=============================================================================
static
KSDATARANGE LoopbackPinDataRangesBridge[] =
{
    {
        sizeof(KSDATARANGE),
        0,
        0,
        0,
        STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
        STATICGUIDOF(KSDATAFORMAT_SUBTYPE_ANALOG),
        STATICGUIDOF(KSDATAFORMAT_SPECIFIER_NONE)
    }
};

static
PKSDATARANGE LoopbackPinDataRangePointersBridge[] =
{
    &LoopbackPinDataRangesBridge[0]
};

//=============================================================================
static
PCPIN_DESCRIPTOR LoopbackWaveMiniportPins[] =
{
    // Wave In Bridge Pin (Capture - From Topology) KSPIN_WAVE_BRIDGE
    {
        0,
        0,
        0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(LoopbackPinDataRangePointersBridge),
            LoopbackPinDataRangePointersBridge,
            KSPIN_DATAFLOW_IN,
            KSPIN_COMMUNICATION_NONE,
            &KSCATEGORY_AUDIO,
            NULL,
            0
        }
    },
    // Wave Out Streaming Pin (Render) KSPIN_WAVE_HOST - This will be the loopback output
    {
        LOOPBACK_MAX_STREAMS,
        LOOPBACK_MAX_STREAMS,
        0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(LoopbackPinDataRangePointersStream),
            LoopbackPinDataRangePointersStream,
            KSPIN_DATAFLOW_OUT,
            KSPIN_COMMUNICATION_SINK,
            &KSCATEGORY_AUDIO,
            &KSAUDFNAME_STEREO_MIX,
            0
        }
    }
};

//=============================================================================
static
PCNODE_DESCRIPTOR LoopbackWaveMiniportNodes[] =
{
    // No specific nodes for a simple loopback.
};

//=============================================================================
static
PCCONNECTION_DESCRIPTOR LoopbackWaveMiniportConnections[] =
{
    { PCFILTER_NODE,        KSPIN_WAVE_BRIDGE,      PCFILTER_NODE,     1 },
};

//=============================================================================
static
PCPROPERTY_ITEM PropertiesLoopbackWaveFilter[] =
{
    {
        &KSPROPSETID_Pin,
        KSPROPERTY_PIN_PROPOSEDATAFORMAT,
        KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_BASICSUPPORT,
        PropertyHandler_WaveFilter
    },
    {
        &KSPROPSETID_Pin,
        KSPROPERTY_PIN_PROPOSEDATAFORMAT2,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
        PropertyHandler_WaveFilter
    }
};

DEFINE_PCAUTOMATION_TABLE_PROP(AutomationLoopbackWaveFilter, PropertiesLoopbackWaveFilter);

//=============================================================================
static
PCFILTER_DESCRIPTOR LoopbackWaveMiniportFilterDescriptor =
{
    0,                                              // Version
    &AutomationLoopbackWaveFilter,                   // AutomationTable
    sizeof(PCPIN_DESCRIPTOR),                       // PinSize
    SIZEOF_ARRAY(LoopbackWaveMiniportPins),          // PinCount
    LoopbackWaveMiniportPins,                        // Pins
    sizeof(PCNODE_DESCRIPTOR),                      // NodeSize
    0,                                              // NodeCount
    NULL,                                           // Nodes
    SIZEOF_ARRAY(LoopbackWaveMiniportConnections),   // ConnectionCount
    LoopbackWaveMiniportConnections,                 // Connections
    0,                                              // CategoryCount
    NULL                                            // Categories
};

#endif // _SIMPLEAUDIOSAMPLE_LOOPBACKWAVTABLE_H_ 