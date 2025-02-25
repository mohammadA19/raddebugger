
public class Arena : BumpAllocator
{
    public int ReserveSize = 64 * 1024 * 1024;

    [Bitfield<bool>(.Public, .Auto, "NoChain")]
	[Bitfield<bool>(.Public, .Auto, "LargePages")]
	int flags;

    public int CommitSize
    {
        get => MaxPoolSize;
        set
        {
            if (value < MinPoolSize)
            {
                MinPoolSize = value;
            }

            MaxPoolSize = value;
        }
    }
}