
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

type Temp struct {
	arena *Arena
	pos uint64
}

func NewArenaWithParams(params *ArenaParams) *Arena {

}

func NewArena() *Arena {
	return NewArenaWithParams( &{ 0, MB(64), KB(64), nil } )
}

func (arena *Arena) Release {

}

func (arena *Arena) Push(size, align uint64) []byte {

}

func (arena *Arena) Pos() uint64 {

}

func (arena *Arena) PopTo(pos uint64) {

}

func (arena *Arena) Clear() {

}

func (arena *Arena) Pop(amount uint64) {

}

func BeginTemp(arena *Arena) Temp {

}

func (temp *Temp) End() {

}

// missing:
// PushArrayNoZeroAligned
// PushArrayAligned

func (arena *Arena) PushArrayNoZero[T any](count uint64) []T {

}

func (arena *Arena) PushArray[T any](count uint64) []T {

}
