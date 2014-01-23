//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// 0.0.1
// Alexey Potehin <gnuplanet@gmail.com>, http://www.gnuplanet.ru/doc/cv
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include "submodule/lib_cpp/lib_cpp.hpp"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// global vars
namespace global
{
	bool flag_le; // little-endian (x86) or big-endian (SPARC)

	uint8_t data_size = 1; // sizeof(uint8_t) or sizeof(uint16_t) or sizeof(uint32_t) or sizeof(uint64_t)

	uint64_t data;

	uint64_t offset;

	bool flag_set = false;

	bool flag_offset = false;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// view bytes in hex
void view_val(const void *p, uint8_t size, bool flag_le)
{
	putchar('0');
	putchar('x');
	const uint8_t *pval = (const uint8_t *)p;

	if (flag_le == true)
	{
		pval += (size - 1);
	}

	for (uint8_t i=0; i < size; i++)
	{
		const char *p = lib_cpp::bin2hex(*pval);

		if (flag_le == true)
		{
			pval--;
		}
		else
		{
			pval++;
		}

		putchar(*p);
		p++;
		putchar(*p);
	}
	putchar('\n');
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// get data
int xget(const char *filename, void *p, uint8_t size)
{
	if (lib_cpp::file_get(filename, global::offset, p, size) == -1)
	{
		printf("ERROR[lib_cpp::file_get()]: %s\n", strerror(errno));
		return -1;
	}
	view_val(p, size, global::flag_le);

	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// set data
int xset(const char *filename, const void *p, uint8_t size)
{
	if (lib_cpp::file_set(filename, global::offset, p, size) == -1)
	{
		printf("ERROR[lib_cpp::file_set()]: %s\n", strerror(errno));
		return -1;
	}
//	view_val(p, size, global::flag_le);

	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// use file
int do_file(const char *filename)
{
	if (global::flag_offset == false)
	{
		printf("ERROR: OFFSET is not set\n");
		return -1;
	}

	uint8_t  data8  = global::data;
	uint16_t data16 = global::data;
	uint32_t data32 = global::data;
	uint64_t data64 = global::data;

	if (global::flag_set == false)
	{
		if (global::data_size == 1)
		{
			return xget(filename, &data8, sizeof(data8));
		}
		if (global::data_size == 2)
		{
			return xget(filename, &data16, sizeof(data16));
		}
		if (global::data_size == 4)
		{
			return xget(filename, &data32, sizeof(data32));
		}
		if (global::data_size == 8)
		{
			return xget(filename, &data64, sizeof(data64));
		}
	}
	else
	{
		if (global::data_size == 1)
		{
			return xset(filename, &data8, sizeof(data8));
		}
		if (global::data_size == 2)
		{
			return xset(filename, &data16, sizeof(data16));
		}
		if (global::data_size == 4)
		{
			return xset(filename, &data32, sizeof(data32));
		}
		if (global::data_size == 8)
		{
			return xset(filename, &data64, sizeof(data64));
		}
	}


	printf("ERROR: internal error\n");
	return -1;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// use stdinput
int do_stdinput()
{
	if (global::flag_offset == false)
	{
		printf("ERROR: OFFSET is not set\n");
		return -1;
	}


	int c;
	uint64_t offset_cur = uint64_t(-1) - global::data_size + 1;
	bool flag_ok = false;
	uint8_t data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };


	for (;;)
	{
		c = getchar();
		if (c == EOF)
		{
			break;
		}


		data[0] = data[1];
		data[1] = data[2];
		data[2] = data[3];
		data[3] = data[4];
		data[4] = data[5];
		data[5] = data[6];
		data[6] = data[7];
		data[7] = (uint8_t)c;


		offset_cur++;
		if (offset_cur == global::offset)
		{
			if (global::data_size == 1)
			{
				view_val(data + (sizeof(uint64_t) - sizeof(uint8_t)), sizeof(uint8_t), global::flag_le);
				flag_ok = true;
				break;
			}

			if (global::data_size == 2)
			{
				view_val(data + (sizeof(uint64_t) - sizeof(uint16_t)), sizeof(uint16_t), global::flag_le);
				flag_ok = true;
				break;
			}

			if (global::data_size == 4)
			{
				view_val(data + (sizeof(uint64_t) - sizeof(uint32_t)), sizeof(uint32_t), global::flag_le);
				flag_ok = true;
				break;
			}

			if (global::data_size == 8)
			{
				view_val(data + (sizeof(uint64_t) - sizeof(uint64_t)), sizeof(uint64_t), global::flag_le);
				flag_ok = true;
				break;
			}

			break;
		}
	}

	if (flag_ok == false)
	{
		printf("ERROR: invalid OFFSET value\n");
		return 1;
	}


	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// view help
void help()
{
	printf("%s\t(%s)\n", PROG_FULL_NAME, PROG_URL);
	printf("example: %s [-le|-be|-u8|-u16|-u32|-u64|-set DATA] -offset OFFSET [FILE] [--]\n", PROG_NAME);
	printf("\n");

	printf("get and set u8/u16/u32/u64 in FILE or stdin.\n");
	printf("\n");

	printf("  -h, -help, --help    this message\n");
	printf("  -le                  use little-endian (x86)\n");
	printf("  -be                  use big-endian (SPARC)\n");
	printf("  -u8                  use data as uint8_t\n");
	printf("  -u16                 use data as uint16_t\n");
	printf("  -u32                 use data as uint32_t\n");
	printf("  -u64                 use data as uint64_t\n");
	printf("  -set DATA            set data value\n");
	printf("  -offset OFFSET       set offset value\n");

	printf("With no FILE, or when FILE is --, read standard input.\n");
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// general function
int main(int argc, char *argv[])
{
	int rc;
	bool flag_file = false;


// set default convertion
	global::flag_le = true;
	uint16_t testendian = 0x0001;
	if (*((uint8_t *)&testendian) == 0)
	{
		global::flag_le = false;
	}


// parse command line args
	for (int i=1; i < argc; i++)
	{
		if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-help") == 0) || (strcmp(argv[i], "-h") == 0))
		{
			help();
			return 1;
		}

		if (strcmp(argv[i], "-le") == 0)
		{
			global::flag_le = true;
			continue;
		}

		if (strcmp(argv[i], "-be") == 0)
		{
			global::flag_le = false;
			continue;
		}

		if (strcmp(argv[i], "-u8") == 0)
		{
			global::data_size = 1;
			continue;
		}

		if (strcmp(argv[i], "-u16") == 0)
		{
			global::data_size = 2;
			continue;
		}

		if (strcmp(argv[i], "-u32") == 0)
		{
			global::data_size = 4;
			continue;
		}

		if (strcmp(argv[i], "-u64") == 0)
		{
			global::data_size = 8;
			continue;
		}

		if (strcmp(argv[i], "-set") == 0)
		{
			i++;
			if (i >= argc)
			{
				printf("ERROR: invalid DATA value\n");
				return 1;
			}

			if (lib_cpp::str2uint(global::data, 0, argv[i]) == false)
			{
				printf("ERROR: invalid DATA value\n");
				return 1;
			}
			global::flag_set = true;
			continue;
		}

		if (strcmp(argv[i], "-offset") == 0)
		{
			i++;
			if (i >= argc)
			{
				printf("ERROR: invalid OFFSET value\n");
				return 1;
			}

			if (lib_cpp::str2uint(global::offset, 0, argv[i]) == false)
			{
				printf("ERROR: invalid OFFSET value\n");
				return 1;
			}
			global::flag_offset = true;
			continue;
		}

		if (strcmp(argv[i], "--") == 0)
		{
			break;
		}


		flag_file = true;
		rc = do_file(argv[i]);
		if (rc == -1) return 1;
	}

	if (flag_file == false)
	{
		rc = do_stdinput();
		if (rc == -1) return 1;
	}


	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
