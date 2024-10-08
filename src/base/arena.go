
package core

const ArenaHeaderSize = 64

type ArenaFlags uint32

const (
	ArenaFlags_NoChain = 1 << 0
	ArenaFlags_LargePages = 1 << 1
)

type ArenaParams struct {
	Flags ArenaFlags
	ReserveSize, CommitSize uint64
	OptionalBackingBuffer []byte
}

type Arena struct {
	prev, current *Arena
	flags ArenaFlags
	cmtSize uint32

	resSize, basePos, pos, cmt, res uint64
}
// static assert : sizeof(Arena) <= ArenaHeaderSize

type ArenaBuilder struct {
	params ArenaParams
}

type Temp struct {
	arena *Arena
	pos uint64
}

func ArenaAllocWithParams(params *ArenaParams) *Arena {

}

func ArenaAlloc() *Arena {
	return ArenaAllocWithParams( &{ 0, MB(64), KB(64), nil } )
}

