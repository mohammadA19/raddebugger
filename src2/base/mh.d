
module mh;

enum int countOf(T) = __traits(allMembers, T).length;

enum membersOf(T) = __traits(allMembers, T);
