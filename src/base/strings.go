
package base

type String8 struct {
    str []uint8
    size uint64
}

type String16 struct {
    str []uint16
    size uint64
}

type String32 struct {
    str []uint32
    size uint64
}

type String8Node struct {
    next *String8Node
    str String8
}

type String8MetaNode struct {
    next *String8MetaNode
    str String8
}

type String8List struct {
    first, last           String8Node
    nodeCount, totalSize  uint64
}

type String8Array struct {
    v      *String8
    count  uint64
}

type StringMatchFlags uint32
const (
    StringMatchFlags_CaseInsensitive  = 1 << 0
    StringMatchFlags_RightSideSloppy  = 1 << 1
    StringMatchFlags_SlashInsensitive = 1 << 2
)

type StringSplitFlags uint32
const (
    StringSplitFlags_KeepEmpties = 1 << 0
)

type PathStyle uint32
const (
    PathStyle_Relative = iota
    PathStyle_WindowsAbsolute
    PathStyle_UnixAbsolute
    // missing: PathStyle_SystemAbsolute
)

type StringJoin struct {
    pre, sep, post  String8
}

type Stirng8TextPtPair struct {
    str  String8
    pt   TextPt
}

type UnicodeDecode struct {
    inc, codepoint  uint32
}

type FuzzyMatchRangeNode struct {
    next *FuzzyMatchRangeNode
    range Range1U64
}

type FuzzyMatchRangeList struct {
    first, last                       *FuzzyMatchRangeNode
    count, needlePartCount, totalDim  uint64
}

