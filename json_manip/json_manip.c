#include <json.h> // JsonParseFile()
#include <sequence.h> // Sequence()
#include <stdio.h> // printf()
#include <stdlib.h> // free()

int main(int argc, const char *const *argv)
{
    Seq *args = SeqFromArgv(argc, argv);
    size_t length = SeqLength(args);
    assert(length == argc);
    Seq *jsons = SeqNew(10 * (argc - 1), JsonDestroy);
    for (int i = 1; i < length; ++i)
    {
        const char *const filename = SeqAt(args, i);
        JsonElement *doc = NULL;
        JsonParseError err = JsonParseFile(filename, 1000, &doc);
        if (err != JSON_PARSE_OK)
        {
            break;
        }
        SeqAppend(jsons, doc);
        JsonElement *copy = JsonCopy(doc);
        SeqAppend(jsons, copy);
        {
            Writer *writer = StringWriter();
            JsonWrite(writer, doc, 0);
            char *output = StringWriterClose(writer);
            printf("%s\n", output);
            free(output);
        }
        {
            Writer *writer = StringWriter();
            JsonWriteCompact(writer, doc);
            char *output = StringWriterClose(writer);
            printf("%s\n", output);
            free(output);
        }
        {
            JsonElement *merged = JsonMerge(doc, copy);
            SeqAppend(jsons, merged);
            JsonObjectMergeDeepInplace(merged, copy);
            if (JsonCompare(doc, copy) != 0)
            {
                printf("Not equal\n");
            }
            else
            {
                printf("Equal\n");
            }
            JsonElement *deep = JsonObjectMergeDeep(doc, copy);
            SeqAppend(jsons, deep);
        }
    }
    SeqDestroy(jsons);
    SeqDestroy(args);
    return 0;
}
