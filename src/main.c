
int main(int argc, char * argv[])
{
    char* buffer = NULL;
    buffer = (char*)malloc(100 * sizeof(char));

    if(fgets(buffer, 100, stdin) == NULL)
        return 1;

    for(int i = 0, j = 0; i <= strlen(buffer); i++)
    {
        if (!isspace(buffer[i]))
        {
            buffer[j] = buffer[i];
            j++;
        }
    }
    buffer = (char*)realloc(buffer, strlen(buffer) * sizeof(char));
    
    if (buffer != NULL)
        free(buffer);
    return 0;
}
