# Contiki-ng port for stm32l-discovery board with at86rf230

## Issues with Contiki-ng

Contiki-ng is not able to build assembly files with \*.s extension (only \*.S).
Some fixes have been made into the main Makefile.include to make this work :

diff --git a/Makefile.include b/Makefile.include
index 0a013f8..6e3a012 100644
--- a/Makefile.include
+++ b/Makefile.include
@@ -57 +57 @@ MODULES += os os/net os/net/mac os/net/routing os/storage
-oname = ${patsubst %.c,%.o,${patsubst %.S,%.o,$(1)}}
+oname = ${patsubst %.c,%.o,${patsubst %.S,%.o,${patsubst %.s,%.o,$(1)}}}
@@ -215,0 +216 @@ vpath %.S $(SOURCEDIRS)
+vpath %.s $(SOURCEDIRS)
@@ -269,0 +271,4 @@ $(OBJECTDIR)/%.o: %.S | $(OBJECTDIR)
+       
+$(OBJECTDIR)/%.o: %.s | $(OBJECTDIR)
+       $(TRACE_AS)
+       $(Q)$(AS) $(ASFLAGS) -o $@ $<

