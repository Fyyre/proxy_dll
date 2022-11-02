.code

extern OrignalD3DPERF_BeginEvent: QWORD
extern OrignalD3DPERF_BeginEvent: QWORD
extern OrignalD3DPERF_EndEvent: QWORD
extern OrignalD3DPERF_GetStatus: QWORD
extern OrignalD3DPERF_QueryRepeatFrame: QWORD
extern OrignalD3DPERF_SetMarker: QWORD
extern OrignalD3DPERF_SetOptions: QWORD
extern OrignalD3DPERF_SetRegion: QWORD
extern OrignalDebugSetLevel: QWORD
extern OrignalDebugSetMute: QWORD
extern OrignalDirect3D9EnableMaximizedWindowedModeShim: QWORD
extern OrignalDirect3DCreate9: QWORD
extern OrignalDirect3DCreate9Ex: QWORD
extern OrignalDirect3DShaderValidatorCreate9: QWORD
extern OrignalPSGPError: QWORD
extern OrignalPSGPSampleTexture: QWORD

public D3DPERF_BeginEvent
public D3DPERF_EndEvent
public D3DPERF_GetStatus
public D3DPERF_QueryRepeatFrame
public D3DPERF_SetMarker
public D3DPERF_SetOptions
public D3DPERF_SetRegion
public DebugSetLevel
public DebugSetMute
public Direct3D9EnableMaximizedWindowedModeShim
public Direct3DCreate9
public Direct3DCreate9Ex
public Direct3DShaderValidatorCreate9
public PSGPError
public PSGPSampleTexture

D3DPERF_BeginEvent:                         jmp [OrignalD3DPERF_BeginEvent]
D3DPERF_EndEvent:                           jmp [OrignalD3DPERF_EndEvent]
D3DPERF_GetStatus:                          jmp [OrignalD3DPERF_GetStatus]
D3DPERF_QueryRepeatFrame:                   jmp [OrignalD3DPERF_QueryRepeatFrame]
D3DPERF_SetMarker:                          jmp [OrignalD3DPERF_SetMarker]
D3DPERF_SetOptions:                         jmp [OrignalD3DPERF_SetOptions]
D3DPERF_SetRegion:                          jmp [OrignalD3DPERF_SetRegion]
DebugSetLevel:                              jmp [OrignalDebugSetLevel]
DebugSetMute:                               jmp [OrignalDebugSetMute]
Direct3D9EnableMaximizedWindowedModeShim:   jmp [OrignalDirect3D9EnableMaximizedWindowedModeShim]
Direct3DCreate9:                            jmp [OrignalDirect3DCreate9]
Direct3DCreate9Ex:                          jmp [OrignalDirect3DCreate9Ex]
Direct3DShaderValidatorCreate9:             jmp [OrignalDirect3DShaderValidatorCreate9]
PSGPError:                                  jmp [OrignalPSGPError]
PSGPSampleTexture:                          jmp [OrignalPSGPSampleTexture]

END