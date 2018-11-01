    	/*do {
    		if (scanf("%c", &symb) == EOF) {
    			printf("Usage: %s <path> <args>\n");
    			return EXIT_FAILURE;
    		}
    		if (symb != ' ' && symb != '\n') {	
    			buf[len++] = symb;
    		} else if (len != 0) {
    		    buf[len] = 0;
    			args[cnt] = (char*)malloc(len);	

    			if (first) {
    				first = 0;
    				str = (char*)malloc(len);
    				strcpy(str, buf);
    				strcpy(args[0], buf);
    			} else {
					strcpy(args[cnt], buf);
    			} 
    			cnt++;
    			len = 0;
    		}          
    	} while (symb != '\n');  */