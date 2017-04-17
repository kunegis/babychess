#include "stream_file.hh"

#include "tmp_dir.hh"
#include "format.hh"

#include <unistd.h>

bool Read_From_File_Helper(const char *filename,
			   void *object,
			   bool (*read_stream) (FILE *, void *))
{
	FILE *file= fopen(filename, "r");
	
	if (file == NULL)
		return false;

	if (! read_stream(file, object))
	{
		fclose(file);
		return false;
	}

	if (0 > fclose(file))
		return false;

	return true;
}


bool Write_To_File_Helper
(const char *filename,
 const void *object, 
 bool (*write_stream) (FILE *, const void *))
{
	String tmp_name= filename;
	tmp_name += ".backup";

	FILE *file= fopen(tmp_name(), "w");
	if (file == NULL)
		return false;

	if (! write_stream(file, object))
		goto close_file;
	if (0 > fclose(file))
		goto unlink;
	if (0 > rename(tmp_name(), filename))  
		return false;

	return true;

 close_file:
	fclose(file);
 unlink:
	unlink(tmp_name());
	return false;
	
}

String Write_To_Tmp_File_Helper
(const void *object, 
 bool (*write_stream) (FILE *, const void *))
{
	String filename= Format("%s/babychess.stream_file.XXXXXX", Tmp_Dir());
	int fd= mkstemp(filename.c());
	if (fd < 0)  return "";

	FILE *file= fdopen(fd, "w");
	if (file == NULL)   goto close_fd;
	
	if (! write_stream(file, object))
	{
		fclose(file);
		goto unlink;
	}
	if (0 > fclose(file))  
		goto unlink;

	return filename();
	
 close_fd:
	close(fd);
 unlink:
	unlink(filename());
	return "";
}
