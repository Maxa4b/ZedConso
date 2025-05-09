# Horloge 100 MHz de la ZedBoard
set_property PACKAGE_PIN Y9 [get_ports clk]
set_property IOSTANDARD LVCMOS33 [get_ports clk]
create_clock -period 10.0 -name sys_clk -waveform {0 5} [get_ports clk]

# Bouton (BTN0)
set_property PACKAGE_PIN P16 [get_ports btn]
set_property IOSTANDARD LVCMOS33 [get_ports btn]

# UART TX (vers ESP32) sur Pmod JA1 pin 1 = Y11
set_property PACKAGE_PIN Y11 [get_ports tx_uart]
set_property IOSTANDARD LVCMOS33 [get_ports tx_uart]
