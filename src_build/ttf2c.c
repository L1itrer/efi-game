#define NOB_IMPLEMENTATION
#include "../src/thirdparty/nob/nob.h"

#include <time.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "../src/thirdparty/stb/stb_truetype.h"

#define WIDTH (64)
#define HEIGHT (2048)
static unsigned char pixels[WIDTH * HEIGHT];

#define CHAR_COUNT 95
#define START_CHAR 32
stbtt_bakedchar cdata[CHAR_COUNT];

void usage(void)
{
  printf("ttf2d USAGE: ttf2c input output name\n");
}

int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    usage();
    return 1;
  }

  const char* input = argv[1];
  const char* output = argv[2];
  const char* fontName = argv[3];
  Nob_String_Builder sb = {0};
  if (!nob_read_entire_file(input, &sb)) return 1;

  float pixelHeight = 50.0f;

  int res = stbtt_BakeFontBitmap((unsigned char*)sb.items, 0, pixelHeight, pixels, WIDTH, HEIGHT, START_CHAR, CHAR_COUNT, cdata);

  printf("res = %d\n", res);

  assert(res > 0);

  sb.count = 0;

  time_t t = time(NULL);
  struct tm currTime = *localtime(&t);

  char s[64] = {0};
  size_t ret = strftime(s, sizeof(s), "%c", &currTime);
  assert(ret);
  nob_sb_appendf(&sb, "// (%s) Meta Generated DO NOT MODIFY MANUALLY\n", s);
  nob_sb_appendf(&sb, "#ifndef %s_FONT\n", fontName);
  nob_sb_appendf(&sb, "#define %s_FONT\n\n", fontName);

  nob_sb_appendf(&sb, "#ifndef FONT_CDATA\n");
  nob_sb_appendf(&sb, "#define FONT_CDATA\n\n");

  nob_sb_appendf(&sb, "typedef struct CharData{\n");
  nob_sb_appendf(&sb, "  unsigned short x0,y0,x1,y1;\n");
  nob_sb_appendf(&sb, "  float xoff,yoff,xadvance;\n");
  nob_sb_appendf(&sb, "}CharData;\n");
  nob_sb_appendf(&sb, "#endif //FONT_CDATA\n\n");

  nob_sb_appendf(&sb, "#define %s_START_CHAR %d\n", fontName, START_CHAR);
  nob_sb_appendf(&sb, "CharData %s_cdata[] = {\n", fontName);
  for (int i = 0;i < CHAR_COUNT;++i)
  {
    nob_sb_appendf(&sb, "{.x0 = %d, .y0 = %d, .x1 = %d, .y1 = %d, .xoff = %f, .yoff = %f, .xadvance = %f},\n",
                   cdata[i].x0, cdata[i].y0, cdata[i].x1, cdata[i].y1,
                   cdata[i].xoff, cdata[i].yoff, cdata[i].xadvance
                   );
  }
  nob_sb_appendf(&sb, "};\n\n");

  nob_sb_appendf(&sb, "#define %s_WIDTH %d\n", fontName, WIDTH);
  nob_sb_appendf(&sb, "#define %s_HEIGHT %d\n", fontName, res);
  nob_sb_appendf(&sb, "unsigned char %s_pixels[] = {\n", fontName);
  for (int y = 0;y < res;++y)
  {
    for (int x = 0;x < WIDTH;++x)
    {
      nob_sb_appendf(&sb, "%3d, ", pixels[y * WIDTH + x]);
    }
    nob_sb_appendf(&sb, "\n");
  }

  nob_sb_appendf(&sb, "};\n");
  nob_sb_appendf(&sb, "#endif //%s_FONT\n", fontName);
  nob_write_entire_file(output, sb.items, sb.count);

  return 0;
}
