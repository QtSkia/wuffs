// Copyright 2017 The Wuffs Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
This test program is typically run indirectly, by the "wuffs test" or "wuffs
bench" commands. These commands take an optional "-mimic" flag to check that
Wuffs' output mimics (i.e. exactly matches) other libraries' output, such as
giflib for GIF, libpng for PNG, etc.

To manually run this test:

for CC in clang gcc; do
  $CC -std=c99 -Wall -Werror gif.c && ./a.out
  rm -f a.out
done

Each edition should print "PASS", amongst other information, and exit(0).

Add the "wuffs mimic cflags" (everything after the colon below) to the C
compiler flags (after the .c file) to run the mimic tests.

To manually run the benchmarks, replace "-Wall -Werror" with "-O3" and replace
the first "./a.out" with "./a.out -bench". Combine these changes with the
"wuffs mimic cflags" to run the mimic benchmarks.
*/

// !! wuffs mimic cflags: -DWUFFS_MIMIC -lgif

// Wuffs ships as a "single file C library" or "header file library" as per
// https://github.com/nothings/stb/blob/master/docs/stb_howto.txt
//
// To use that single file as a "foo.c"-like implementation, instead of a
// "foo.h"-like header, #define WUFFS_IMPLEMENTATION before #include'ing or
// compiling it.
#define WUFFS_IMPLEMENTATION

// Defining the WUFFS_CONFIG__MODULE* macros are optional, but it lets users of
// release/c/etc.h whitelist which parts of Wuffs to build. That file contains
// the entire Wuffs standard library, implementing a variety of codecs and file
// formats. Without this macro definition, an optimizing compiler or linker may
// very well discard Wuffs code for unused codecs, but listing the Wuffs
// modules we use makes that process explicit. Preprocessing means that such
// code simply isn't compiled.
#define WUFFS_CONFIG__MODULES
#define WUFFS_CONFIG__MODULE__BASE
#define WUFFS_CONFIG__MODULE__GIF
#define WUFFS_CONFIG__MODULE__LZW

// If building this program in an environment that doesn't easily accommodate
// relative includes, you can use the script/inline-c-relative-includes.go
// program to generate a stand-alone C file.
#include "../../../release/c/wuffs-unsupported-snapshot.h"
#include "../testlib/testlib.c"
#ifdef WUFFS_MIMIC
#include "../mimiclib/gif.c"
#endif

// ---------------- Basic Tests

const char* test_basic_bad_receiver() {
  CHECK_FOCUS(__func__);
  wuffs_base__image_config ic = ((wuffs_base__image_config){});
  wuffs_base__io_reader src = ((wuffs_base__io_reader){});
  const char* z = wuffs_gif__decoder__decode_image_config(NULL, &ic, src);
  if (z != wuffs_base__error__bad_receiver) {
    RETURN_FAIL("decode_image_config: got \"%s\", want \"%s\"", z,
                wuffs_base__error__bad_receiver);
  }
  return NULL;
}

const char* test_basic_bad_sizeof_receiver() {
  CHECK_FOCUS(__func__);
  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  const char* z =
      wuffs_gif__decoder__check_wuffs_version(&dec, 0, WUFFS_VERSION);
  if (z != wuffs_base__error__bad_sizeof_receiver) {
    RETURN_FAIL("decode_image_config: got \"%s\", want \"%s\"", z,
                wuffs_base__error__bad_sizeof_receiver);
  }
  return NULL;
}

const char* test_basic_bad_wuffs_version() {
  CHECK_FOCUS(__func__);
  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  const char* z = wuffs_gif__decoder__check_wuffs_version(
      &dec, sizeof dec, WUFFS_VERSION ^ 0x123456789ABC);
  if (z != wuffs_base__error__bad_wuffs_version) {
    RETURN_FAIL("decode_image_config: got \"%s\", want \"%s\"", z,
                wuffs_base__error__bad_wuffs_version);
  }
  return NULL;
}

const char* test_basic_check_wuffs_version_not_called() {
  CHECK_FOCUS(__func__);
  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  wuffs_base__image_config ic = ((wuffs_base__image_config){});
  wuffs_base__io_reader src = ((wuffs_base__io_reader){});
  const char* z = wuffs_gif__decoder__decode_image_config(&dec, &ic, src);
  if (z != wuffs_base__error__check_wuffs_version_missing) {
    RETURN_FAIL("decode_image_config: got \"%s\", want \"%s\"", z,
                wuffs_base__error__check_wuffs_version_missing);
  }
  return NULL;
}

const char* test_basic_status_is_error() {
  CHECK_FOCUS(__func__);
  if (wuffs_base__status__is_error(NULL)) {
    RETURN_FAIL("is_error(NULL) returned true");
  }
  if (!wuffs_base__status__is_error(wuffs_base__error__bad_wuffs_version)) {
    RETURN_FAIL("is_error(BAD_WUFFS_VERSION) returned false");
  }
  if (wuffs_base__status__is_error(wuffs_base__suspension__short_write)) {
    RETURN_FAIL("is_error(SHORT_WRITE) returned true");
  }
  if (!wuffs_base__status__is_error(wuffs_gif__error__bad_header)) {
    RETURN_FAIL("is_error(BAD_HEADER) returned false");
  }
  return NULL;
}

const char* test_basic_status_strings() {
  CHECK_FOCUS(__func__);
  const char* s1 = wuffs_base__error__bad_wuffs_version;
  const char* t1 = "?base: bad wuffs version";
  if (strcmp(s1, t1)) {
    RETURN_FAIL("got \"%s\", want \"%s\"", s1, t1);
  }
  const char* s2 = wuffs_base__suspension__short_write;
  const char* t2 = "$base: short write";
  if (strcmp(s2, t2)) {
    RETURN_FAIL("got \"%s\", want \"%s\"", s2, t2);
  }
  const char* s3 = wuffs_gif__error__bad_header;
  const char* t3 = "?gif: bad header";
  if (strcmp(s3, t3)) {
    RETURN_FAIL("got \"%s\", want \"%s\"", s3, t3);
  }
  return NULL;
}

const char* test_basic_status_used_package() {
  CHECK_FOCUS(__func__);
  // The function call here is from "std/gif" but the argument is from
  // "std/lzw". The former package depends on the latter.
  const char* s0 = wuffs_lzw__error__bad_code;
  const char* t0 = "?lzw: bad code";
  if (strcmp(s0, t0)) {
    RETURN_FAIL("got \"%s\", want \"%s\"", s0, t0);
  }
  return NULL;
}

const char* test_basic_sub_struct_initializer() {
  CHECK_FOCUS(__func__);
  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  const char* z =
      wuffs_gif__decoder__check_wuffs_version(&dec, sizeof dec, WUFFS_VERSION);
  if (z) {
    RETURN_FAIL("check_wuffs_version: \"%s\"", z);
  }
  if (dec.private_impl.magic != WUFFS_BASE__MAGIC) {
    RETURN_FAIL("outer magic: got %" PRIu32 ", want %" PRIu32 "",
                dec.private_impl.magic, WUFFS_BASE__MAGIC);
  }
  if (dec.private_impl.f_lzw.private_impl.magic != WUFFS_BASE__MAGIC) {
    RETURN_FAIL("inner magic: got %" PRIu32 ", want %" PRIu32,
                dec.private_impl.f_lzw.private_impl.magic, WUFFS_BASE__MAGIC);
  }
  return NULL;
}

// ---------------- GIF Tests

const char* wuffs_gif_decode(wuffs_base__io_buffer* dst,
                             wuffs_base__io_buffer* src) {
  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  const char* z =
      wuffs_gif__decoder__check_wuffs_version(&dec, sizeof dec, WUFFS_VERSION);
  if (z) {
    return z;
  }

  wuffs_base__image_config ic = ((wuffs_base__image_config){});
  wuffs_base__frame_config fc = ((wuffs_base__frame_config){});
  wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(src);
  z = wuffs_gif__decoder__decode_image_config(&dec, &ic, src_reader);
  if (z) {
    return z;
  }

  wuffs_base__pixel_buffer pb = ((wuffs_base__pixel_buffer){});
  z = wuffs_base__pixel_buffer__set_from_slice(&pb, &ic.pixcfg,
                                               global_pixel_slice);
  if (z) {
    return z;
  }

  uint64_t workbuf_len = wuffs_base__image_config__workbuf_len(&ic).max_incl;
  if (workbuf_len > BUFFER_SIZE) {
    return "work buffer size is too large";
  }
  wuffs_base__slice_u8 workbuf = ((wuffs_base__slice_u8){
      .ptr = global_work_array,
      .len = workbuf_len,
  });

  while (true) {
    z = wuffs_gif__decoder__decode_frame_config(&dec, &fc, src_reader);
    if (z == wuffs_base__warning__end_of_data) {
      break;
    } else if (z) {
      return z;
    }

    z = wuffs_gif__decoder__decode_frame(&dec, &pb, src_reader, workbuf, NULL);
    if (z) {
      return z;
    }

    z = copy_to_io_buffer_from_pixel_buffer(
        dst, &pb, wuffs_base__frame_config__bounds(&fc));
    if (z) {
      return z;
    }
  }
  return NULL;
}

const char* do_test_wuffs_gif_decode(const char* filename,
                                     const char* palette_filename,
                                     const char* indexes_filename,
                                     uint64_t rlimit) {
  wuffs_base__io_buffer got = ((wuffs_base__io_buffer){
      .data = global_got_slice,
  });
  wuffs_base__io_buffer src = ((wuffs_base__io_buffer){
      .data = global_src_slice,
  });

  const char* z = read_file(&src, filename);
  if (z) {
    return z;
  }

  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  z = wuffs_gif__decoder__check_wuffs_version(&dec, sizeof dec, WUFFS_VERSION);
  if (z) {
    RETURN_FAIL("check_wuffs_version: \"%s\"", z);
  }

  wuffs_base__frame_config fc = ((wuffs_base__frame_config){});
  wuffs_base__pixel_buffer pb = ((wuffs_base__pixel_buffer){});

  {
    wuffs_base__image_config ic = ((wuffs_base__image_config){});
    wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(&src);
    z = wuffs_gif__decoder__decode_image_config(&dec, &ic, src_reader);
    if (z) {
      RETURN_FAIL("decode_image_config: got \"%s\"", z);
    }
    if (wuffs_base__pixel_config__pixel_format(&ic.pixcfg) !=
        WUFFS_BASE__PIXEL_FORMAT__INDEXED__BGRA_NONPREMUL) {
      RETURN_FAIL("pixel_format: got 0x%08" PRIX32 ", want 0x%08" PRIX32,
                  wuffs_base__pixel_config__pixel_format(&ic.pixcfg),
                  WUFFS_BASE__PIXEL_FORMAT__INDEXED__BGRA_NONPREMUL);
    }

    // bricks-dither.gif is a 160 × 120, opaque, still (not animated) GIF.
    if (wuffs_base__pixel_config__width(&ic.pixcfg) != 160) {
      RETURN_FAIL("width: got %" PRIu32 ", want 160",
                  wuffs_base__pixel_config__width(&ic.pixcfg));
    }
    if (wuffs_base__pixel_config__height(&ic.pixcfg) != 120) {
      RETURN_FAIL("height: got %" PRIu32 ", want 120",
                  wuffs_base__pixel_config__height(&ic.pixcfg));
    }
    if (wuffs_base__image_config__workbuf_len(&ic).max_incl != 160) {
      RETURN_FAIL("workbuf_len: got %" PRIu64 ", want 160",
                  wuffs_base__image_config__workbuf_len(&ic).max_incl);
    }
    if (!wuffs_base__image_config__first_frame_is_opaque(&ic)) {
      RETURN_FAIL("first_frame_is_opaque: got false, want true");
    }
    z = wuffs_base__pixel_buffer__set_from_slice(&pb, &ic.pixcfg,
                                                 global_pixel_slice);
    if (z) {
      RETURN_FAIL("set_from_slice: \"%s\"", z);
    }

    uint32_t got = wuffs_gif__decoder__num_animation_loops(&dec);
    if (got != 1) {
      RETURN_FAIL("num_animation_loops: got %" PRIu32 ", want 1", got);
    }
  }

  wuffs_base__slice_u8 workbuf = ((wuffs_base__slice_u8){
      .ptr = global_work_array,
      .len = 160,
  });

  int num_iters = 0;
  while (true) {
    num_iters++;
    wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(&src);
    if (rlimit) {
      set_reader_limit(&src_reader, rlimit);
    }
    size_t old_ri = src.meta.ri;

    z = wuffs_gif__decoder__decode_frame_config(&dec, &fc, src_reader);

    if (!z) {
      break;
    }
    if (z != wuffs_base__suspension__short_read) {
      RETURN_FAIL("decode_frame_config: got \"%s\", want \"%s\"", z,
                  wuffs_base__suspension__short_read);
    }

    if (src.meta.ri < old_ri) {
      RETURN_FAIL("read index src.meta.ri went backwards");
    }
    if (src.meta.ri == old_ri) {
      RETURN_FAIL("no progress was made");
    }
  }

  while (true) {
    num_iters++;
    wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(&src);
    if (rlimit) {
      set_reader_limit(&src_reader, rlimit);
    }
    size_t old_ri = src.meta.ri;

    const char* z =
        wuffs_gif__decoder__decode_frame(&dec, &pb, src_reader, workbuf, NULL);

    if (!z) {
      break;
    }
    if (z != wuffs_base__suspension__short_read) {
      RETURN_FAIL("decode_frame: got \"%s\", want \"%s\"", z,
                  wuffs_base__suspension__short_read);
    }

    if (src.meta.ri < old_ri) {
      RETURN_FAIL("read index src.meta.ri went backwards");
    }
    if (src.meta.ri == old_ri) {
      RETURN_FAIL("no progress was made");
    }
  }

  z = copy_to_io_buffer_from_pixel_buffer(
      &got, &pb, wuffs_base__frame_config__bounds(&fc));
  if (z) {
    return z;
  }

  if (rlimit) {
    if (num_iters <= 2) {
      RETURN_FAIL("num_iters: got %d, want > 2", num_iters);
    }
  } else {
    if (num_iters != 2) {
      RETURN_FAIL("num_iters: got %d, want 2", num_iters);
    }
  }

  wuffs_base__io_buffer pal_got = ((wuffs_base__io_buffer){
      .data = wuffs_base__pixel_buffer__palette(&pb),
  });
  pal_got.meta.wi = pal_got.data.len;
  uint8_t pal_want_array[1024];
  wuffs_base__io_buffer pal_want = ((wuffs_base__io_buffer){
      .data = ((wuffs_base__slice_u8){
          .ptr = pal_want_array,
          .len = 1024,
      }),
  });
  z = read_file(&pal_want, palette_filename);
  if (z) {
    return z;
  }
  z = check_io_buffers_equal("palette ", &pal_got, &pal_want);
  if (z) {
    return z;
  }

  wuffs_base__io_buffer ind_want = ((wuffs_base__io_buffer){
      .data = global_want_slice,
  });
  z = read_file(&ind_want, indexes_filename);
  if (z) {
    return z;
  }
  z = check_io_buffers_equal("indexes ", &got, &ind_want);
  if (z) {
    return z;
  }

  {
    if (src.meta.ri == src.meta.wi) {
      RETURN_FAIL("decode_frame returned \"ok\" but src was exhausted");
    }
    wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(&src);
    z = wuffs_gif__decoder__decode_frame(&dec, &pb, src_reader, workbuf, NULL);
    if (z != wuffs_base__warning__end_of_data) {
      RETURN_FAIL("decode_frame: got \"%s\", want \"%s\"", z,
                  wuffs_base__warning__end_of_data);
    }
    if (src.meta.ri != src.meta.wi) {
      RETURN_FAIL(
          "decode_frame returned \"end of data\" but src was not exhausted");
    }
  }

  return NULL;
}

const char* test_wuffs_gif_call_sequence() {
  CHECK_FOCUS(__func__);

  wuffs_base__io_buffer src = ((wuffs_base__io_buffer){
      .data = global_src_slice,
  });

  const char* z = read_file(&src, "../../data/bricks-dither.gif");
  if (z) {
    return z;
  }

  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  z = wuffs_gif__decoder__check_wuffs_version(&dec, sizeof dec, WUFFS_VERSION);
  if (z) {
    RETURN_FAIL("check_wuffs_version: \"%s\"", z);
  }

  wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(&src);

  z = wuffs_gif__decoder__decode_image_config(&dec, NULL, src_reader);
  if (z) {
    RETURN_FAIL("decode_image_config: got \"%s\"", z);
  }

  z = wuffs_gif__decoder__decode_image_config(&dec, NULL, src_reader);
  if (z != wuffs_base__error__bad_call_sequence) {
    RETURN_FAIL("decode_image_config: got \"%s\", want \"%s\"", z,
                wuffs_base__error__bad_call_sequence);
  }
  return NULL;
}

const char* do_test_wuffs_gif_decode_animated(
    const char* filename,
    uint32_t want_num_loops,
    uint32_t want_num_frames,
    wuffs_base__rect_ie_u32* want_frame_config_bounds) {
  wuffs_base__io_buffer src = ((wuffs_base__io_buffer){
      .data = global_src_slice,
  });

  const char* z = read_file(&src, filename);
  if (z) {
    return z;
  }

  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  z = wuffs_gif__decoder__check_wuffs_version(&dec, sizeof dec, WUFFS_VERSION);
  if (z) {
    RETURN_FAIL("check_wuffs_version: \"%s\"", z);
  }

  wuffs_base__image_config ic = ((wuffs_base__image_config){});
  wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(&src);

  z = wuffs_gif__decoder__decode_image_config(&dec, &ic, src_reader);
  if (z) {
    RETURN_FAIL("decode_image_config: got \"%s\"", z);
  }

  uint64_t workbuf_len = wuffs_base__image_config__workbuf_len(&ic).max_incl;
  if (workbuf_len > BUFFER_SIZE) {
    return "work buffer size is too large";
  }
  wuffs_base__slice_u8 workbuf = ((wuffs_base__slice_u8){
      .ptr = global_work_array,
      .len = workbuf_len,
  });

  uint32_t got_num_loops = wuffs_gif__decoder__num_animation_loops(&dec);
  if (got_num_loops != want_num_loops) {
    RETURN_FAIL("num_loops: got %" PRIu32 ", want %" PRIu32, got_num_loops,
                want_num_loops);
  }

  wuffs_base__pixel_buffer pb = ((wuffs_base__pixel_buffer){});
  z = wuffs_base__pixel_buffer__set_from_slice(&pb, &ic.pixcfg,
                                               global_pixel_slice);
  if (z) {
    RETURN_FAIL("set_from_slice: \"%s\"", z);
  }

  uint32_t i;
  for (i = 0; i < want_num_frames; i++) {
    wuffs_base__frame_config fc = ((wuffs_base__frame_config){});
    z = wuffs_gif__decoder__decode_frame_config(&dec, &fc, src_reader);
    if (z) {
      RETURN_FAIL("decode_frame_config #%" PRIu32 ": got \"%s\"", i, z);
    }

    if (want_frame_config_bounds) {
      wuffs_base__rect_ie_u32 got = wuffs_base__frame_config__bounds(&fc);
      wuffs_base__rect_ie_u32 want = want_frame_config_bounds[i];
      if (!wuffs_base__rect_ie_u32__equals(&got, want)) {
        RETURN_FAIL("decode_frame_config #%" PRIu32 ": bounds: got (%" PRIu32
                    ", %" PRIu32 ")-(%" PRIu32 ", %" PRIu32 "), want (%" PRIu32
                    ", %" PRIu32 ")-(%" PRIu32 ", %" PRIu32 ")",
                    i, got.min_incl_x, got.min_incl_y, got.max_excl_x,
                    got.max_excl_y, want.min_incl_x, want.min_incl_y,
                    want.max_excl_x, want.max_excl_y);
      }
    }

    z = wuffs_gif__decoder__decode_frame(&dec, &pb, src_reader, workbuf, NULL);
    if (z) {
      RETURN_FAIL("decode_frame #%" PRIu32 ": got \"%s\"", i, z);
    }
  }

  // There should be no more frames, no matter how many times we call
  // decode_frame.
  for (i = 0; i < 3; i++) {
    z = wuffs_gif__decoder__decode_frame(&dec, &pb, src_reader, workbuf, NULL);
    if (z != wuffs_base__warning__end_of_data) {
      RETURN_FAIL("decode_frame: got \"%s\", want \"%s\"", z,
                  wuffs_base__warning__end_of_data);
    }
  }

  uint64_t got_num_frames = wuffs_gif__decoder__num_decoded_frames(&dec);
  if (got_num_frames != want_num_frames) {
    RETURN_FAIL("frame_count: got %" PRIu64 ", want %" PRIu32, got_num_frames,
                want_num_frames);
  }

  return NULL;
}

const char* test_wuffs_gif_decode_animated_big() {
  CHECK_FOCUS(__func__);
  return do_test_wuffs_gif_decode_animated("../../data/gifplayer-muybridge.gif",
                                           0, 380, NULL);
}

const char* test_wuffs_gif_decode_animated_medium() {
  CHECK_FOCUS(__func__);
  return do_test_wuffs_gif_decode_animated("../../data/muybridge.gif", 0, 15,
                                           NULL);
}

const char* test_wuffs_gif_decode_animated_small() {
  CHECK_FOCUS(__func__);
  // animated-red-blue.gif's num_loops should be 3. The value explicitly in the
  // wire format is 0x0002, but that value means "repeat 2 times after the
  // first play", so the total number of loops is 3.
  const uint32_t want_num_loops = 3;
  wuffs_base__rect_ie_u32 want_frame_config_bounds[4] = {
      make_rect_ie_u32(0, 0, 64, 48),
      make_rect_ie_u32(15, 31, 52, 40),
      make_rect_ie_u32(15, 0, 64, 40),
      make_rect_ie_u32(15, 0, 64, 40),
  };
  return do_test_wuffs_gif_decode_animated(
      "../../data/animated-red-blue.gif", want_num_loops,
      WUFFS_TESTLIB_ARRAY_SIZE(want_frame_config_bounds),
      want_frame_config_bounds);
}

const char* test_wuffs_gif_decode_frame_out_of_bounds() {
  CHECK_FOCUS(__func__);
  wuffs_base__io_buffer src = ((wuffs_base__io_buffer){
      .data = global_src_slice,
  });
  const char* z =
      read_file(&src, "../../data/artificial/gif-frame-out-of-bounds.gif");
  if (z) {
    return z;
  }

  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  z = wuffs_gif__decoder__check_wuffs_version(&dec, sizeof dec, WUFFS_VERSION);
  if (z) {
    RETURN_FAIL("check_wuffs_version: \"%s\"", z);
  }
  wuffs_base__image_config ic = ((wuffs_base__image_config){});
  wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(&src);
  z = wuffs_gif__decoder__decode_image_config(&dec, &ic, src_reader);
  if (z) {
    RETURN_FAIL("decode_image_config: \"%s\"", z);
  }

  // The nominal width and height for the overall image is 2×2, but its first
  // frame extends those bounds to 4×2. See
  // test/data/artificial/gif-frame-out-of-bounds.gif.make-artificial.txt for
  // more discussion.

  const uint32_t width = 4;
  const uint32_t height = 2;

  if (wuffs_base__pixel_config__width(&ic.pixcfg) != width) {
    RETURN_FAIL("width: got %" PRIu32 ", want %" PRIu32,
                wuffs_base__pixel_config__width(&ic.pixcfg), width);
  }
  if (wuffs_base__pixel_config__height(&ic.pixcfg) != height) {
    RETURN_FAIL("height: got %" PRIu32 ", want %" PRIu32,
                wuffs_base__pixel_config__height(&ic.pixcfg), height);
  }

  wuffs_base__pixel_buffer pb = ((wuffs_base__pixel_buffer){});
  z = wuffs_base__pixel_buffer__set_from_slice(&pb, &ic.pixcfg,
                                               global_pixel_slice);
  if (z) {
    RETURN_FAIL("set_from_slice: \"%s\"", z);
  }

  uint64_t workbuf_len = wuffs_base__image_config__workbuf_len(&ic).max_incl;
  if (workbuf_len > BUFFER_SIZE) {
    RETURN_FAIL("work buffer size is too large");
  }
  wuffs_base__slice_u8 workbuf = ((wuffs_base__slice_u8){
      .ptr = global_work_array,
      .len = workbuf_len,
  });

  // See test/data/artificial/gif-frame-out-of-bounds.gif.make-artificial.txt
  // for the want_frame_config_bounds and want_pixel_indexes source.

  wuffs_base__rect_ie_u32 want_frame_config_bounds[4] = {
      make_rect_ie_u32(1, 0, 4, 1),
      make_rect_ie_u32(0, 1, 2, 2),
      make_rect_ie_u32(0, 2, 1, 2),
      make_rect_ie_u32(2, 0, 4, 2),
  };

  const char* want_pixel_indexes[4] = {
      ".123....",
      "....89..",
      "........",
      "..45..89",
  };

  uint32_t i;
  for (i = 0; true; i++) {
    {
      wuffs_base__frame_config fc = ((wuffs_base__frame_config){});
      z = wuffs_gif__decoder__decode_frame_config(&dec, &fc, src_reader);
      if (i == WUFFS_TESTLIB_ARRAY_SIZE(want_frame_config_bounds)) {
        if (z != wuffs_base__warning__end_of_data) {
          RETURN_FAIL("decode_frame_config #%" PRIu32 ": got \"%s\"", i, z);
        }
        break;
      }
      if (z) {
        RETURN_FAIL("decode_frame_config #%" PRIu32 ": got \"%s\"", i, z);
      }

      wuffs_base__rect_ie_u32 got = wuffs_base__frame_config__bounds(&fc);
      wuffs_base__rect_ie_u32 want = want_frame_config_bounds[i];
      if (!wuffs_base__rect_ie_u32__equals(&got, want)) {
        RETURN_FAIL("decode_frame_config #%" PRIu32 ": bounds: got (%" PRIu32
                    ", %" PRIu32 ")-(%" PRIu32 ", %" PRIu32 "), want (%" PRIu32
                    ", %" PRIu32 ")-(%" PRIu32 ", %" PRIu32 ")",
                    i, got.min_incl_x, got.min_incl_y, got.max_excl_x,
                    got.max_excl_y, want.min_incl_x, want.min_incl_y,
                    want.max_excl_x, want.max_excl_y);
      }
    }

    {
      wuffs_base__table_u8 p = wuffs_base__pixel_buffer__plane(&pb, 0);
      uint32_t y, x;
      for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
          p.ptr[(y * p.stride) + x] = 0;
        }
      }

      z = wuffs_gif__decoder__decode_frame(&dec, &pb, src_reader, workbuf,
                                           NULL);
      if (z) {
        RETURN_FAIL("decode_frame #%" PRIu32 ": got \"%s\"", i, z);
      }

      char got[(width * height) + 1];
      for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
          uint8_t index = 0x0F & p.ptr[(y * p.stride) + x];
          got[(y * width) + x] = index ? ('0' + index) : '.';
        }
      }
      got[width * height] = 0;

      const char* want = want_pixel_indexes[i];
      if (memcmp(got, want, width * height)) {
        RETURN_FAIL("decode_frame #%" PRIu32 ": got \"%s\", want \"%s\"", i,
                    got, want);
      }
    }
  }
  return NULL;
}

const char* test_wuffs_gif_decode_input_is_a_gif() {
  CHECK_FOCUS(__func__);
  return do_test_wuffs_gif_decode("../../data/bricks-dither.gif",
                                  "../../data/bricks-dither.palette",
                                  "../../data/bricks-dither.indexes", 0);
}

const char* test_wuffs_gif_decode_input_is_a_gif_many_big_reads() {
  CHECK_FOCUS(__func__);
  return do_test_wuffs_gif_decode("../../data/bricks-dither.gif",
                                  "../../data/bricks-dither.palette",
                                  "../../data/bricks-dither.indexes", 4096);
}

const char* test_wuffs_gif_decode_input_is_a_gif_many_medium_reads() {
  CHECK_FOCUS(__func__);
  return do_test_wuffs_gif_decode("../../data/bricks-dither.gif",
                                  "../../data/bricks-dither.palette",
                                  "../../data/bricks-dither.indexes", 787);
  // The magic 787 tickles being in the middle of a decode_extension skip
  // call.
  //
  // TODO: has 787 changed since we decode the image_config separately?
}

const char* test_wuffs_gif_decode_input_is_a_gif_many_small_reads() {
  CHECK_FOCUS(__func__);
  return do_test_wuffs_gif_decode("../../data/bricks-dither.gif",
                                  "../../data/bricks-dither.palette",
                                  "../../data/bricks-dither.indexes", 13);
}

const char* test_wuffs_gif_decode_input_is_a_png() {
  CHECK_FOCUS(__func__);

  wuffs_base__io_buffer src = ((wuffs_base__io_buffer){
      .data = global_src_slice,
  });

  const char* z = read_file(&src, "../../data/bricks-dither.png");
  if (z) {
    return z;
  }

  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  z = wuffs_gif__decoder__check_wuffs_version(&dec, sizeof dec, WUFFS_VERSION);
  if (z) {
    RETURN_FAIL("check_wuffs_version: \"%s\"", z);
  }
  wuffs_base__image_config ic = ((wuffs_base__image_config){});
  wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(&src);

  z = wuffs_gif__decoder__decode_image_config(&dec, &ic, src_reader);
  if (z != wuffs_gif__error__bad_header) {
    RETURN_FAIL("decode_image_config: got \"%s\", want \"%s\"", z,
                wuffs_gif__error__bad_header);
  }
  return NULL;
}

const char* test_wuffs_gif_decode_multiple_loop_counts() {
  CHECK_FOCUS(__func__);
  wuffs_base__io_buffer src = ((wuffs_base__io_buffer){
      .data = global_src_slice,
  });
  const char* z =
      read_file(&src, "../../data/artificial/gif-multiple-loop-counts.gif");
  if (z) {
    return z;
  }

  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  z = wuffs_gif__decoder__check_wuffs_version(&dec, sizeof dec, WUFFS_VERSION);
  if (z) {
    RETURN_FAIL("check_wuffs_version: \"%s\"", z);
  }
  wuffs_base__image_config ic = ((wuffs_base__image_config){});
  wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(&src);
  z = wuffs_gif__decoder__decode_image_config(&dec, &ic, src_reader);
  if (z) {
    RETURN_FAIL("decode_image_config: \"%s\"", z);
  }

  // See test/data/artificial/gif-multiple-loop-counts.gif.make-artificial.txt
  // for the want_num_animation_loops source. Note that the GIF wire format's
  // loop counts are the number of animation loops *after* the first play
  // through, and Wuffs' are the number *including* the first play through.

  uint32_t want_num_animation_loops[4] = {1, 1, 51, 31};

  uint32_t i;
  for (i = 0; true; i++) {
    {
      z = wuffs_gif__decoder__decode_frame_config(&dec, NULL, src_reader);
      if (i == WUFFS_TESTLIB_ARRAY_SIZE(want_num_animation_loops)) {
        if (z != wuffs_base__warning__end_of_data) {
          RETURN_FAIL("decode_frame_config #%" PRIu32 ": got \"%s\"", i, z);
        }
        break;
      }
      if (z) {
        RETURN_FAIL("decode_frame_config #%" PRIu32 ": got \"%s\"", i, z);
      }
    }

    uint32_t got = wuffs_gif__decoder__num_animation_loops(&dec);
    uint32_t want = want_num_animation_loops[i];
    if (got != want) {
      RETURN_FAIL("num_animation_loops #%" PRIu32 ": got %" PRIu32
                  ", want %" PRIu32,
                  i, got, want);
    }
  }

  uint32_t got = wuffs_gif__decoder__num_animation_loops(&dec);
  uint32_t want = 41;
  if (got != want) {
    RETURN_FAIL("num_animation_loops #%" PRIu32 ": got %" PRIu32
                ", want %" PRIu32,
                i, got, want);
  }
  return NULL;
}

const char* do_test_wuffs_gif_num_decoded(bool frame_config) {
  wuffs_base__io_buffer src = ((wuffs_base__io_buffer){
      .data = global_src_slice,
  });

  const char* z = read_file(&src, "../../data/animated-red-blue.gif");
  if (z) {
    return z;
  }

  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  z = wuffs_gif__decoder__check_wuffs_version(&dec, sizeof dec, WUFFS_VERSION);
  if (z) {
    RETURN_FAIL("check_wuffs_version: \"%s\"", z);
  }
  wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(&src);

  wuffs_base__pixel_buffer pb = ((wuffs_base__pixel_buffer){});
  if (!frame_config) {
    wuffs_base__image_config ic = ((wuffs_base__image_config){});
    z = wuffs_gif__decoder__decode_image_config(&dec, &ic, src_reader);
    if (z) {
      RETURN_FAIL("decode_image_config: \"%s\"", z);
    }

    z = wuffs_base__pixel_buffer__set_from_slice(&pb, &ic.pixcfg,
                                                 global_pixel_slice);
    if (z) {
      RETURN_FAIL("set_from_slice: \"%s\"", z);
    }
  }

  wuffs_base__slice_u8 workbuf = ((wuffs_base__slice_u8){
      .ptr = global_work_array,
      .len = 64,
  });

  const char* method = frame_config ? "decode_frame_config" : "decode_frame";
  bool end_of_data = false;
  uint64_t want = 0;
  while (true) {
    uint64_t got =
        frame_config
            ? (uint64_t)(wuffs_gif__decoder__num_decoded_frame_configs(&dec))
            : (uint64_t)(wuffs_gif__decoder__num_decoded_frames(&dec));
    if (got != want) {
      RETURN_FAIL("num_%ss: got %" PRIu64 ", want %" PRIu64, method, got, want);
    }

    if (end_of_data) {
      break;
    }

    const char* z = NULL;
    if (frame_config) {
      z = wuffs_gif__decoder__decode_frame_config(&dec, NULL, src_reader);
    } else {
      z = wuffs_gif__decoder__decode_frame(&dec, &pb, src_reader, workbuf,
                                           NULL);
    }

    if (!z) {
      want++;
    } else if (z == wuffs_base__warning__end_of_data) {
      end_of_data = true;
    } else {
      RETURN_FAIL("%s: \"%s\"", method, z);
    }
  }

  if (want != 4) {
    RETURN_FAIL("%s: got %" PRIu64 ", want 4", method, want);
  }
  return NULL;
}

const char* test_wuffs_gif_num_decoded_frame_configs() {
  CHECK_FOCUS(__func__);
  return do_test_wuffs_gif_num_decoded(true);
}

const char* test_wuffs_gif_num_decoded_frames() {
  CHECK_FOCUS(__func__);
  return do_test_wuffs_gif_num_decoded(false);
}

const char* do_test_wuffs_gif_io_position(bool chunked) {
  wuffs_base__io_buffer src = ((wuffs_base__io_buffer){
      .data = global_src_slice,
  });
  const char* z = read_file(&src, "../../data/animated-red-blue.gif");
  if (z) {
    return z;
  }

  wuffs_gif__decoder dec = ((wuffs_gif__decoder){});
  z = wuffs_gif__decoder__check_wuffs_version(&dec, sizeof dec, WUFFS_VERSION);
  if (z) {
    RETURN_FAIL("check_wuffs_version: \"%s\"", z);
  }

  wuffs_base__io_reader src_reader = wuffs_base__io_buffer__reader(&src);

  if (chunked) {
    if (src.meta.wi < 50) {
      RETURN_FAIL("src is too short");
    }
    size_t saved_wi = src.meta.wi;
    bool saved_closed = src.meta.closed;
    src.meta.wi = 30;
    src.meta.closed = false;

    z = wuffs_gif__decoder__decode_image_config(&dec, NULL, src_reader);
    if (z != wuffs_base__suspension__short_read) {
      RETURN_FAIL("decode_image_config (chunked): \"%s\"", z);
    }

    src.meta.wi = saved_wi;
    src.meta.closed = saved_closed;

    if (src.meta.pos != 0) {
      RETURN_FAIL("src.meta.pos: got %" PRIu64 ", want zero", src.meta.pos);
    }
    wuffs_base__io_buffer__compact(&src);
    if (src.meta.pos == 0) {
      RETURN_FAIL("src.meta.pos: got %" PRIu64 ", want non-zero", src.meta.pos);
    }
  }

  z = wuffs_gif__decoder__decode_image_config(&dec, NULL, src_reader);
  if (z) {
    RETURN_FAIL("decode_image_config: \"%s\"", z);
  }

  wuffs_base__frame_config fcs[4];
  uint32_t width_wants[4] = {64, 37, 49, 49};
  uint64_t pos_wants[4] = {781, 2126, 2187, 2542};
  int i;
  for (i = 0; i < 4; i++) {
    fcs[i] = ((wuffs_base__frame_config){});
    z = wuffs_gif__decoder__decode_frame_config(&dec, &fcs[i], src_reader);
    if (z) {
      RETURN_FAIL("decode_frame_config #%d: \"%s\"", i, z);
    }

    uint64_t index_got = wuffs_base__frame_config__index(&fcs[i]);
    uint64_t index_want = i;
    if (index_got != index_want) {
      RETURN_FAIL("index #%d: got %" PRIu64 ", want %" PRIu64, i, index_got,
                  index_want);
    }

    uint32_t width_got = wuffs_base__frame_config__width(&fcs[i]);
    uint32_t width_want = width_wants[i];
    if (width_got != width_want) {
      RETURN_FAIL("width #%d: got %" PRIu32 ", want %" PRIu32, i, width_got,
                  width_want);
    }

    uint64_t pos_got = wuffs_base__frame_config__io_position(&fcs[i]);
    uint64_t pos_want = pos_wants[i];
    if (pos_got != pos_want) {
      RETURN_FAIL("io_position #%d: got %" PRIu64 ", want %" PRIu64, i, pos_got,
                  pos_want);
    }

    // Look for the 0x21 byte that's a GIF's Extension Introducer. Not every
    // GIF's frame_config's I/O position will point to 0x21, as an 0x2C Image
    // Separator is also valid. But for animated-red-blue.gif, it'll be 0x21.
    if (pos_got < src.meta.pos) {
      RETURN_FAIL("io_position #%d: got %" PRIu64 ", was too small", i,
                  pos_got);
    }
    uint64_t src_ptr_offset = pos_got - src.meta.pos;
    if (src_ptr_offset >= src.meta.wi) {
      RETURN_FAIL("io_position #%d: got %" PRIu64 ", was too large", i,
                  pos_got);
    }
    uint8_t x = src.data.ptr[src_ptr_offset];
    if (x != 0x21) {
      RETURN_FAIL("Image Descriptor byte #%d: got 0x%02X, want 0x2C", i,
                  (int)x);
    }
  }

  z = wuffs_gif__decoder__decode_frame_config(&dec, NULL, src_reader);
  if (z != wuffs_base__warning__end_of_data) {
    RETURN_FAIL("decode_frame_config EOD: \"%s\"", z);
  }

  // If we're chunked, we've discarded some source bytes due to an earlier
  // wuffs_base__io_buffer__compact call. We won't bother testing
  // wuffs_gif__decoder__restart_frame in that case.
  if (chunked) {
    return NULL;
  }

  for (i = 0; i < 4; i++) {
    src.meta.ri = pos_wants[i];

    z = wuffs_gif__decoder__restart_frame(
        &dec, i, wuffs_base__frame_config__io_position(&fcs[i]));
    if (z) {
      RETURN_FAIL("restart_frame #%d: \"%s\"", i, z);
    }

    int j;
    for (j = i; j < 4; j++) {
      wuffs_base__frame_config fc = ((wuffs_base__frame_config){});
      z = wuffs_gif__decoder__decode_frame_config(&dec, &fc, src_reader);
      if (z) {
        RETURN_FAIL("decode_frame_config #%d, #%d: \"%s\"", i, j, z);
      }

      uint64_t index_got = wuffs_base__frame_config__index(&fc);
      uint64_t index_want = j;
      if (index_got != index_want) {
        RETURN_FAIL("index #%d, #%d: got %" PRIu64 ", want %" PRIu64, i, j,
                    index_got, index_want);
      }

      uint32_t width_got = wuffs_base__frame_config__width(&fc);
      uint32_t width_want = width_wants[j];
      if (width_got != width_want) {
        RETURN_FAIL("width #%d, #%d: got %" PRIu32 ", want %" PRIu32, i, j,
                    width_got, width_want);
      }
    }

    z = wuffs_gif__decoder__decode_frame_config(&dec, NULL, src_reader);
    if (z != wuffs_base__warning__end_of_data) {
      RETURN_FAIL("decode_frame_config EOD #%d: \"%s\"", i, z);
    }
  }

  return NULL;
}

const char* test_wuffs_gif_io_position_one_chunk() {
  CHECK_FOCUS(__func__);
  return do_test_wuffs_gif_io_position(false);
}

const char* test_wuffs_gif_io_position_two_chunks() {
  CHECK_FOCUS(__func__);
  return do_test_wuffs_gif_io_position(true);
}

  // ---------------- Mimic Tests

#ifdef WUFFS_MIMIC

const char* do_test_mimic_gif_decode(const char* filename) {
  wuffs_base__io_buffer src = ((wuffs_base__io_buffer){
      .data = global_src_slice,
  });
  const char* z = read_file(&src, filename);
  if (z) {
    return z;
  }

  src.meta.ri = 0;
  wuffs_base__io_buffer got = ((wuffs_base__io_buffer){
      .data = global_got_slice,
  });
  z = wuffs_gif_decode(&got, &src);
  if (z) {
    return z;
  }

  src.meta.ri = 0;
  wuffs_base__io_buffer want = ((wuffs_base__io_buffer){
      .data = global_want_slice,
  });
  z = mimic_gif_decode(&want, &src);
  if (z) {
    return z;
  }

  z = check_io_buffers_equal("", &got, &want);
  if (z) {
    return z;
  }

  // TODO: check the palette RGB values, not just the palette indexes
  // (pixels).

  return NULL;
}

const char* test_mimic_gif_decode_animated_small() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/animated-red-blue.gif");
}

const char* test_mimic_gif_decode_bricks_dither() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/bricks-dither.gif");
}

const char* test_mimic_gif_decode_bricks_gray() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/bricks-gray.gif");
}

const char* test_mimic_gif_decode_bricks_nodither() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/bricks-nodither.gif");
}

const char* test_mimic_gif_decode_gifplayer_muybridge() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/gifplayer-muybridge.gif");
}

const char* test_mimic_gif_decode_harvesters() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/harvesters.gif");
}

const char* test_mimic_gif_decode_hat() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/hat.gif");
}

const char* test_mimic_gif_decode_hibiscus() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/hibiscus.gif");
}

const char* test_mimic_gif_decode_hippopotamus_interlaced() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/hippopotamus.interlaced.gif");
}

const char* test_mimic_gif_decode_hippopotamus_regular() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/hippopotamus.regular.gif");
}

const char* test_mimic_gif_decode_muybridge() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/muybridge.gif");
}

const char* test_mimic_gif_decode_pjw_thumbnail() {
  CHECK_FOCUS(__func__);
  return do_test_mimic_gif_decode("../../data/pjw-thumbnail.gif");
}

#endif  // WUFFS_MIMIC

// ---------------- GIF Benches

const char* do_bench_gif_decode(
    const char* (*decode_func)(wuffs_base__io_buffer*, wuffs_base__io_buffer*),
    const char* filename,
    uint64_t iters_unscaled) {
  wuffs_base__io_buffer got = ((wuffs_base__io_buffer){
      .data = global_got_slice,
  });
  wuffs_base__io_buffer src = ((wuffs_base__io_buffer){
      .data = global_src_slice,
  });

  const char* z = read_file(&src, filename);
  if (z) {
    return z;
  }

  bench_start();
  uint64_t n_bytes = 0;
  uint64_t i;
  uint64_t iters = iters_unscaled * iterscale;
  for (i = 0; i < iters; i++) {
    got.meta.wi = 0;
    src.meta.ri = 0;
    z = decode_func(&got, &src);
    if (z) {
      return z;
    }
    n_bytes += got.meta.wi;
  }
  bench_finish(iters, n_bytes);
  return NULL;
}

const char* bench_wuffs_gif_decode_1k_bw() {
  CHECK_FOCUS(__func__);
  return do_bench_gif_decode(wuffs_gif_decode, "../../data/pjw-thumbnail.gif",
                             2000);
}

const char* bench_wuffs_gif_decode_1k_color() {
  CHECK_FOCUS(__func__);
  return do_bench_gif_decode(wuffs_gif_decode,
                             "../../data/hippopotamus.regular.gif", 1000);
}

const char* bench_wuffs_gif_decode_10k() {
  CHECK_FOCUS(__func__);
  return do_bench_gif_decode(wuffs_gif_decode, "../../data/hat.gif", 100);
}

const char* bench_wuffs_gif_decode_100k() {
  CHECK_FOCUS(__func__);
  return do_bench_gif_decode(wuffs_gif_decode, "../../data/hibiscus.gif", 10);
}

const char* bench_wuffs_gif_decode_1000k() {
  CHECK_FOCUS(__func__);
  return do_bench_gif_decode(wuffs_gif_decode, "../../data/harvesters.gif", 1);
}

  // ---------------- Mimic Benches

#ifdef WUFFS_MIMIC

const char* bench_mimic_gif_decode_1k_bw() {
  CHECK_FOCUS(__func__);
  return do_bench_gif_decode(mimic_gif_decode, "../../data/pjw-thumbnail.gif",
                             2000);
}

const char* bench_mimic_gif_decode_1k_color() {
  CHECK_FOCUS(__func__);
  return do_bench_gif_decode(mimic_gif_decode,
                             "../../data/hippopotamus.regular.gif", 1000);
}

const char* bench_mimic_gif_decode_10k() {
  CHECK_FOCUS(__func__);
  return do_bench_gif_decode(mimic_gif_decode, "../../data/hat.gif", 100);
}

const char* bench_mimic_gif_decode_100k() {
  CHECK_FOCUS(__func__);
  return do_bench_gif_decode(mimic_gif_decode, "../../data/hibiscus.gif", 10);
}

const char* bench_mimic_gif_decode_1000k() {
  CHECK_FOCUS(__func__);
  return do_bench_gif_decode(mimic_gif_decode, "../../data/harvesters.gif", 1);
}

#endif  // WUFFS_MIMIC

// ---------------- Manifest

// The empty comments forces clang-format to place one element per line.
proc tests[] = {

    // These basic tests are really testing the Wuffs compiler. They aren't
    // specific to the std/gif code, but putting them here is as good as any
    // other place.
    test_basic_bad_receiver,                    //
    test_basic_bad_sizeof_receiver,             //
    test_basic_bad_wuffs_version,               //
    test_basic_check_wuffs_version_not_called,  //
    test_basic_status_is_error,                 //
    test_basic_status_strings,                  //
    test_basic_status_used_package,             //
    test_basic_sub_struct_initializer,          //

    test_wuffs_gif_call_sequence,                            //
    test_wuffs_gif_decode_animated_big,                      //
    test_wuffs_gif_decode_animated_medium,                   //
    test_wuffs_gif_decode_animated_small,                    //
    test_wuffs_gif_decode_frame_out_of_bounds,               //
    test_wuffs_gif_decode_input_is_a_gif,                    //
    test_wuffs_gif_decode_input_is_a_gif_many_big_reads,     //
    test_wuffs_gif_decode_input_is_a_gif_many_medium_reads,  //
    test_wuffs_gif_decode_input_is_a_gif_many_small_reads,   //
    test_wuffs_gif_decode_input_is_a_png,                    //
    test_wuffs_gif_decode_multiple_loop_counts,              //
    test_wuffs_gif_num_decoded_frame_configs,                //
    test_wuffs_gif_num_decoded_frames,                       //
    test_wuffs_gif_io_position_one_chunk,                    //
    test_wuffs_gif_io_position_two_chunks,                   //

#ifdef WUFFS_MIMIC

    test_mimic_gif_decode_animated_small,           //
    test_mimic_gif_decode_bricks_dither,            //
    test_mimic_gif_decode_bricks_gray,              //
    test_mimic_gif_decode_bricks_nodither,          //
    test_mimic_gif_decode_gifplayer_muybridge,      //
    test_mimic_gif_decode_harvesters,               //
    test_mimic_gif_decode_hat,                      //
    test_mimic_gif_decode_hibiscus,                 //
    test_mimic_gif_decode_hippopotamus_interlaced,  //
    test_mimic_gif_decode_hippopotamus_regular,     //
    test_mimic_gif_decode_muybridge,                //
    test_mimic_gif_decode_pjw_thumbnail,            //

#endif  // WUFFS_MIMIC

    NULL,
};

// The empty comments forces clang-format to place one element per line.
proc benches[] = {

    bench_wuffs_gif_decode_1k_bw,     //
    bench_wuffs_gif_decode_1k_color,  //
    bench_wuffs_gif_decode_10k,       //
    bench_wuffs_gif_decode_100k,      //
    bench_wuffs_gif_decode_1000k,     //

#ifdef WUFFS_MIMIC

    bench_mimic_gif_decode_1k_bw,     //
    bench_mimic_gif_decode_1k_color,  //
    bench_mimic_gif_decode_10k,       //
    bench_mimic_gif_decode_100k,      //
    bench_mimic_gif_decode_1000k,     //

#endif  // WUFFS_MIMIC

    NULL,
};

int main(int argc, char** argv) {
  proc_package_name = "std/gif";
  return test_main(argc, argv, tests, benches);
}
