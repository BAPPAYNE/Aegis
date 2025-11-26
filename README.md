# Aegis
Aegis is a Windows-based remote system monitoring and administration tool that captures system diagnostics, network information, and screen activity for authorized IT operations and endpoint management. It enables centralized command execution and telemetry collection across distributed systems through a C2-based architecture for enterprise surveillance and compliance purposes.

For ethical use only.

## Features
- **System Diagnostics**: Collects detailed information about system hardware, software, and configurations.
- **Remote Command Execution**: Allows authorized users to execute commands on remote systems.
- **Capture Screenshots**: Periodically captures screenshots of the remote system's display.
- **Network Information**: Gathers data about network connections and configurations.
- VirusTotal : https://www.virustotal.com/gui/file/4c214fcd2ce000d60018fb0d0b303ec54462b2f132d0908d63252f01e248c599/

## Requirements
- Windows Operating System
- MSVC++ or GCC/G++ Redistributable

## Usage
1. Clone the repository:
   ```bash
   git clone https://github.com/BAPPAYNE/Ageis.git
   cd Ageis
   ```
   or download the ZIP and extract it.
2. Build the project using your preferred IDE or build tool. (I have used Microsoft Visual Studio 2022 / MSVC++).

3. Run the compiled executable on the target Windows system via </br>
    cmd/powershell (if you want to pass IP and PORT dynamically as a runtime argument).</br>
    Or you can hardcode the C2 IP and PORT in the source code (main.c) before compiling.
    
## Disclaimer
This tool is intended for ethical and authorized use only. Unauthorized use of this software on systems without explicit permission is illegal and unethical. The author is not responsible for any misuse or damage caused by this tool.
    
## License
This project is licensed under the GNU GENERAL PUBLIC LICENSE Version 3 - see the [LICENSE](LICENSE) file for details.

<!--
## Contribution
Contributions are welcome! Please fork the repository and submit a pull request with your improvements or bug fixes.
-->
