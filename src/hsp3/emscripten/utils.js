var hsp3;

(function (hsp3) {
  // Read / Write memory

  /**
   * Writes a byte value to the specified memory address.
   * @param {number} ptr - The memory address.
   * @param {number} value - The byte value to write.
   */
  hsp3.poke = function (ptr, value) {
    Module.HEAPU8[ptr] = value;
  };

  /**
   * Writes a word (2 bytes) value to the specified memory address.
   * @param {number} ptr - The memory address.
   * @param {number} value - The word value to write.
   */
  hsp3.wpoke = function (ptr, value) {
    Module.HEAPU8[ptr] = value & 0xff;
    Module.HEAPU8[ptr + 1] = (value >> 8) & 0xff;
  };

  /**
   * Writes a long (4 bytes) value to the specified memory address.
   * @param {number} ptr - The memory address.
   * @param {number} value - The long value to write.
   */
  hsp3.lpoke = function (ptr, value) {
    Module.HEAPU8[ptr] = value & 0xff;
    Module.HEAPU8[ptr + 1] = (value >> 8) & 0xff;
    Module.HEAPU8[ptr + 2] = (value >> 16) & 0xff;
    Module.HEAPU8[ptr + 3] = (value >> 24) & 0xff;
  };

  /**
   * Writes a float value to the specified memory address.
   * @param {number} ptr - The memory address.
   * @param {number} value - The float value to write.
   * @throws {Error} If the pointer is not aligned to 4 bytes.
   */
  hsp3.fpoke = function (ptr, value) {
    if (ptr & 3) {
      throw new Error("fpoke: unaligned pointer");
    }
    Module.HEAPF32[ptr >> 2] = value;
  };

  /**
   * Writes a double value to the specified memory address.
   * @param {number} ptr - The memory address.
   * @param {number} value - The double value to write.
   * @throws {Error} If the pointer is not aligned to 8 bytes.
   */
  hsp3.dpoke = function (ptr, value) {
    if (ptr & 7) {
      throw new Error("dpoke: unaligned pointer");
    }
    Module.HEAPF64[ptr >> 3] = value;
  };

  /**
   * Reads a byte value from the specified memory address.
   * @param {number} ptr - The memory address.
   * @returns {number} The byte value read.
   */
  hsp3.peek = function (ptr) {
    return Module.HEAPU8[ptr];
  };

  /**
   * Reads a word (2 bytes) value from the specified memory address.
   * @param {number} ptr - The memory address.
   * @returns {number} The word value read.
   */
  hsp3.wpeek = function (ptr) {
    return Module.HEAPU8[ptr] | (Module.HEAPU8[ptr + 1] << 8);
  };

  /**
   * Reads a long (4 bytes) value from the specified memory address.
   * @param {number} ptr - The memory address.
   * @returns {number} The long value read.
   */
  hsp3.lpeek = function (ptr) {
    return (
      Module.HEAPU8[ptr] |
      (Module.HEAPU8[ptr + 1] << 8) |
      (Module.HEAPU8[ptr + 2] << 16) |
      (Module.HEAPU8[ptr + 3] << 24)
    );
  };

  /**
   * Reads a float value from the specified memory address.
   * @param {number} ptr - The memory address.
   * @returns {number} The float value read.
   * @throws {Error} If the pointer is not aligned to 4 bytes.
   */
  hsp3.fpeek = function (ptr) {
    if (ptr & 3) {
      throw new Error("fpeek: unaligned pointer");
    }
    return Module.HEAPF32[ptr >> 2];
  };

  /**
   * Reads a double value from the specified memory address.
   * @param {number} ptr - The memory address.
   * @returns {number} The double value read.
   * @throws {Error} If the pointer is not aligned to 8 bytes.
   */
  hsp3.dpeek = function (ptr) {
    if (ptr & 7) {
      throw new Error("dpeek: unaligned pointer");
    }
    return Module.HEAPF64[ptr >> 3];
  };

  /**
   * Writes a string to the specified memory address.
   * @param {number} ptr - The memory address.
   * @param {string} str - The string to copy.
   * @param {number} max_bytes - The maximum bytes of the string
   * @throws {Error} If the arguments are invalid.
   */
  hsp3.write_str = function (ptr, str, max_bytes) {
    if (typeof ptr !== "number") {
      throw new Error("strcpy: invalid argument");
    }
    if (typeof str !== "string") {
      throw new Error("strcpy: invalid argument");
    }
    if (typeof max_bytes !== "number") {
      throw new Error("strcpy: invalid argument");
    }
    stringToUTF8(str, ptr, max_bytes);
  };

  /**
   * Reads a string from the specified memory address.
   * @param {number} ptr - The memory address.
   * @returns {string} The string read.
   */
  hsp3.read_str = function (ptr) {
    return UTF8ToString(ptr);
  };

  // File I/O

  /**
   * Writes data to a file at the specified path.
   * @param {string} path - The file path.
   * @param {Uint8Array} data - The data to write.
   */
  hsp3.write_file = function (path, data) {
    let dirs = path.split("/");
    if (dirs.length > 1) {
      for (let i = 1; i < dirs.length; i++) {
        if (dirs[i - 1] === "") continue;
        let dir = dirs.slice(0, i).join("/");
        if (!FS.analyzePath(dir).exists) {
          console.log("mkdir", dir);
          FS.mkdir(dir);
        }
      }
    }
    FS.writeFile(path, data);
  };

  /**
   * Reads data from a file at the specified path.
   * @param {string} path - The file path.
   * @returns {Uint8Array} The data read from the file.
   */
  hsp3.read_file = function (path) {
    return FS.readFile(path);
  };
})(hsp3 || (hsp3 = {}));
