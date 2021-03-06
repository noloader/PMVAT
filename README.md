# PMVAT

The Process Mutex Vulnerability Assessment Tool (PMVAT) can be used to analyze and disrupt IPC and synchronization between userland programs and privileged components like drivers and the kernel. The tool was created by Jeffrey Walton and Brooke Stephens.

PMVAT was developed for testing Windows Antivirus software. The tests involved manipulating shared Windows mutexes, like a mutex that used to guard access to a file that the AV scanner (privileged component) wants to scan.

PMVAT suscessfully attacked 22 or 23 Windows antivirus suites. The tool can attack and squat on named and unnamed mutexes. The tool also has an automated testing mode to test all mutexes in a process. An example of the UI can be seen in the paper (discussed next).

A paper is available called "Old Dogs and New Tricks - Do You Know Where Your Handles Are?" The paper is included with the source code. The paper shows how PEVAT and PMVAT were used to silently shutdown Microsoft Security Essentials, and block a UI window informing the user of the shutdown.

A file called ProtectProcess.rar is available. The sample code in ProtectProcess shows you how to secure your process's DACL to help avoid the manipulations. The code mimics a Windows [Protected Process](http://download.microsoft.com/download/a/f/7/af7777e5-7dcd-4800-8a0a-b18336565f5b/process_vista.doc), which was introduced for Digital Rights Management (DRM) and protecting rich content. However, the code can only protect against unprivileged users; administrators can always gain access in a discretionary access control systems like Windows.