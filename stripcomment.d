import std.stdio;
import std.string;
import std.file;


void main(string[] args)
{
	auto text=cast(string)std.file.read(args[1]);
	bool comment=false;
	int ifdef=0;
	foreach(line;text.splitLines)
	{
		if (line.strip.length==0)
			continue;
		if (line.strip.startsWith("#include <aerospike"))
			continue;
		if (line.strip.startsWith("#pragma"))
			continue;
		if (line.strip.startsWith("#ifdef"))
		{
			ifdef++;
			continue;
		}
		if (line.strip.startsWith("#endif"))
		{
			ifdef--;
			continue;
		}

		if (ifdef>0)
			continue;
		bool repeat=false;
		do
		{
			if (comment)
			{
				auto i=line.indexOf("*/");
				if (i==-1)
				{
					repeat=false;
					continue;
				}
				auto butt=(i==-1||i==line.length-2)?"":line[i+2..$];
				auto j=butt.indexOf("/*");
				if (j!=-1)
				{
					writeln(butt[0..j]);
					comment=true;
					line=line[j..$];
					repeat=true;
				}
				else
				{
					writeln(butt);
					repeat=false;
					comment=false;
				}
			}
			else
			{
				auto i=line.indexOf("/*");
				if (i==-1)
				{
					writeln(line);
					repeat=false;
					continue;
				}
				auto stub=line[0..i];
				auto butt=(i==line.length-2)?"":line[i+2..$];
				auto j=butt.indexOf("*/");
				if (j!=-1)
				{
					writeln(stub);
					line=(j>=butt.length-3)?"":butt[j+2..$];
					repeat=true;
					comment=false;
				}
				else {
					writeln(stub);
					repeat=false;
					comment=true;
				}
			}
		if (line.strip.length==0)
			repeat=false;
		} while(line.strip.length>0 && repeat);
	}
}
