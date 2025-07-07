/*++

Copyright (c) Microsoft Corporation All Rights Reserved

Module Name:

    loopbacktoptable.h

Abstract:

    Declaration of topology miniport tables for the loopback endpoint.

--*/

#ifndef _SIMPLEAUDIOSAMPLE_LOOPBACKTOPTABLE_H_
#define _SIMPLEAUDIOSAMPLE_LOOPBACKTOPTABLE_H_

//=============================================================================
static
KSPIN_DATAFLOW LoopbackTopoPinDataFlows[] =
{
    KSPIN_DATAFLOW_IN,      // From Bridge
    KSPIN_DATAFLOW_OUT      // To Bridge
};

//=============================================================================
static
KSPIN_COMMUNICATION LoopbackTopoPinCommunications[] =
{
    KSPIN_COMMUNICATION_NONE,
    KSPIN_COMMUNICATION_NONE
};

//=============================================================================
static
GUID LoopbackTopoPinCategories[] =
{
    KSCATEGORY_AUDIO,
    KSCATEGORY_AUDIO
};

//=============================================================================
static
KSPIN_DESCRIPTOR_EX LoopbackTopoMiniportPins[] =
{
    {
      0,                                                // InstanceCount
      0,                                                // NulInstances
      0,                                                // MappingPairCount
      NULL,                                             // MappingPairs
      NULL,                                             // AutomationTable
      {                                                 // KsPinDescriptor
        0,                                              // InterfacesCount
        NULL,                                           // Interfaces
        0,                                              // MediumsCount
        NULL,                                           // Mediums
        0,                                              // DataRangesCount
        NULL,                                           // DataRanges
        KSPIN_DATAFLOW_IN,                              // DataFlow
        KSPIN_COMMUNICATION_NONE,                       // Communication
        &KSCATEGORY_AUDIO,                              // Category
        NULL,                                           // Name
        0                                               // Reserved
      }
    },
    {
      0,
      0,
      0,
      NULL,
      NULL,
      {
        0,
        NULL,
        0,
        NULL,
        0,
        NULL,
        KSPIN_DATAFLOW_OUT,
        KSPIN_COMMUNICATION_NONE,
        &KSCATEGORY_AUDIO,
        NULL,
        0
      }
    }
};

//=============================================================================
static
PCNODE_DESCRIPTOR LoopbackTopologyNodes[] =
{
    // No specific nodes for a simple pass-through.
};

//=============================================================================
static
PCCONNECTION_DESCRIPTOR LoopbackTopoMiniportConnections[] =
{
    // FromNode, FromPin, ToNode, ToPin
    // Direct connection from input pin to output pin
    { PCFILTER_NODE, 0, PCFILTER_NODE, 1 }
};

//=============================================================================
static
PCPROPERTY_ITEM PropertiesLoopbackTopoFilter[] =
{
    // No specific properties for a simple pass-through.
};

DEFINE_PCAUTOMATION_TABLE_PROP(AutomationLoopbackTopoFilter, PropertiesLoopbackTopoFilter);

//=============================================================================
static
PCFILTER_DESCRIPTOR LoopbackTopoMiniportFilterDescriptor =
{
  0,                                            // Version
  &AutomationLoopbackTopoFilter,                // AutomationTable
  sizeof(PCPIN_DESCRIPTOR),                     // PinSize
  SIZEOF_ARRAY(LoopbackTopoMiniportPins),       // PinCount
  LoopbackTopoMiniportPins,                     // Pins
  sizeof(PCNODE_DESCRIPTOR),                    // NodeSize
  SIZEOF_ARRAY(LoopbackTopologyNodes),          // NodeCount
  LoopbackTopologyNodes,                        // Nodes
  SIZEOF_ARRAY(LoopbackTopoMiniportConnections),// ConnectionCount
  LoopbackTopoMiniportConnections,              // Connections
  0,                                            // CategoryCount
  NULL                                          // Categories
};

#endif // _SIMPLEAUDIOSAMPLE_LOOPBACKTOPTABLE_H_ 