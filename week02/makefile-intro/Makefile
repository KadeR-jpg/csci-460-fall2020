# credit: https://endler.dev/2017/makefiles/
.PHONY: clean

dress: trousers shoes jacket
	@echo "All done. Let's go otside!\n" >> dressed.txt

jacket: pullover
	@echo "Putting on jacket.\n" >> dressed.txt

pullover: shirt
	@echo "Putting on pullover.\n" >> dressed.txt

shirt:
	@echo "Putting on shirt.\n" >> dressed.txt

trousers: underpants
	@echo "Putting on trousers.\n" >> dressed.txt

underpants:
	@echo "Putting on underpants.\n" >> dressed.txt

shoes: socks
	@echo "Putting on shoes.\n" >> dressed.txt

socks: pullover
	@echo "Putting on socks.\n" >> dressed.txt
	
clean: 
	rm *.txt
	@echo "Cleaning up..."
