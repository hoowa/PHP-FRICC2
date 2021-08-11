FRICC2

---



1. What's FRICC2?
   FRICC2 is a PHP Script encryption tool. When you are developing a commercial software using PHP, the script can be distributed as encrypted up until just before execution. Thanks for php-screw and screw-plus project.

   
   
2. Requirement

   1. PHP 7.X
      NOTICE: PHP must be compiled with the "--with-zlib" option.
      Check that PHP has zlib compiled in it with the PHP script:
      "<? gzopen(); ?>"
      If PHP knows about the function you can happily proceed.
      
   2. UNIX like OS (LINUX, FreeBSD, etc. are included) .

      

3. Installation

   Get Source

   ```bash
   git clone https://github.com/hoowa/FRICC2.git
   cd FRICC2
   ```

   Change KEY / TAG

   ```bash
   # Edit friccload2/friccload2.h
   
   # Recommend to Modify Crypto KEY
   # Example new key is 1,2,3,4,5,6,7,8
   short friccload_cryptokey[] = {
     1,2,3,4,5,6,7,8
   };
   
   # Recommend to Modify FRICCTAG_STR and FRICCTAG_LEN for encrypted script new header
   # Example new tag is HOOWA
   #define FRICCTAG_STR    "HOOWA"
   #define FRICCTAG_LEN    5
   ```

   Build fricc2 (Script encrypter)

   ```bash
   # fricc2/ is encrypter
   cd fricc2/
   make
   # (optional) copy fricc2 to /usr/bin
   cp fricc2 /usr/bin/
   # Syntax to encrypt php script code
   # Usage: ./fricc2 -o [target] [source]
   ```

   Build friccload2 (PHP Extension)

   ```bash
   # friccload2 is PHP Extension written by C Language
   cd friccload2/
   phpize
   ./configure
   make
   make install
   
   # Append this line to php.ini
   extension=friccload2.so
   
   # Restart the web server / your php daemon
   
   # Test in CLI
   php -m|grep friccload2
   ```

   

4. How To Use

   ```bash
   # Create test.php
   <?php
   echo "hello fricc2\n";
   ?>
   
   # Encrypt it
   fricc2 -o test_new.php test.php
   
   # Execute Test
   php ./test_new.php
   # Screen output:
   hello fricc2
   ```

   

5. Copyright
   Sun Bing <hoowa.sun@gmail.com>

