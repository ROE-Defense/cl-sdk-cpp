# Nim FFI binding example for cl-sdk-cpp C-core
# Proving zero-overhead interop with game engine stacks

const
  MaxChannels = 59

type
  ClConfig* {.bycopy.} = object
    apiKey*: cstring
    endpointUrl*: cstring
    useWebsockets*: bool
    engineTickRate*: cint
    enableInterpolation*: bool

  ClSpikeEvent* {.bycopy.} = object
    timestamp*: uint32
    channelId*: uint8
    amplitude*: cfloat

  ClOpticalFlow* {.bycopy.} = object
    timestamp*: uint32
    flowX*: array[MaxChannels, cfloat]
    flowY*: array[MaxChannels, cfloat]

  ClContext = object # Opaque pointer type

{.push header: "../include/cl_sdk.h", cdecl.}
proc cl_init*(config: ptr ClConfig): ptr ClContext
proc cl_destroy*(ctx: ptr ClContext)
proc cl_connect*(ctx: ptr ClContext): bool
proc cl_send_optical_flow*(ctx: ptr ClContext, flow: ptr ClOpticalFlow): bool
proc cl_receive_spikes*(ctx: ptr ClContext, spikesOut: ptr ClSpikeEvent, maxSpikes: cint): cint
{.pop.}

when isMainModule:
  var config = ClConfig(
    apiKey: "nim_test_key",
    endpointUrl: "wss://api.corticallabs.com/v1/dish",
    useWebsockets: true
  )
  
  let ctx = cl_init(addr config)
  if ctx.isNil:
    quit("Failed to initialize SDK")
    
  if not cl_connect(ctx):
    quit("Failed to connect")
    
  echo "[Nim] Successfully connected via Nim FFI to the C-core."
  cl_destroy(ctx)