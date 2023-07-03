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
            printf("Not valid JSON");
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
            JsonElement *merged = NULL;

            // According to asserts, JsonMerge can only be used on 2
            // containers of the same type:
            // works on 2 objects:
            if (JsonGetElementType(doc) == JsonGetElementType(copy)
                && JsonGetElementType(doc) == JSON_ELEMENT_TYPE_CONTAINER)
            {
                // TODO investigate if these asserts in JsonMerge should be
                // kept.
                merged = JsonMerge(doc, copy);
                SeqAppend(jsons, merged);

                // According to asserts, JsonObjectMergeDeep[Inplace] only
                // works on 2 objects:
                if (JsonGetType(merged) == JSON_TYPE_OBJECT
                    && JsonGetType(copy) == JSON_TYPE_OBJECT)
                {
                    JsonObjectMergeDeepInplace(merged, copy);
                    JsonElement *deep = JsonObjectMergeDeep(doc, copy);
                    SeqAppend(jsons, deep);
                }
            }
            if (JsonCompare(doc, copy) != 0)
            {
                printf("Not equal\n");
            }
            else
            {
                printf("Equal\n");
            }
        }
    }
    SeqDestroy(jsons);
    SeqDestroy(args);
    return 0;
}
