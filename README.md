# straw
Straw is a juce automation framework for integration testing.

## Example of REST API

```sh
# Test if a component exists
curl -X GET http://localhost:8001/straw/component/exists -H 'Content-Type: application/json' -d '{"id":"animation"}'

# Return the informations from a component (recursive as well)
curl -X GET http://localhost:8001/straw/component/info -H 'Content-Type: application/json' -d '{"id":"animation", "recursive": true}'

# Click a component
curl -X GET http://localhost:8001/straw/component/click -H 'Content-Type: application/json' -d '{"id":"button"}'

# Render a component (with or without children and return a png)
curl -X GET http://localhost:8001/straw/component/render -H 'Content-Type: application/json' -d '{"id":"animation", "withChildren":true}' > test.png

# Execute custom defined callback
curl -X GET http://localhost:8001/change_background_colour -H 'Content-Type: application/json' -d '{"colour":"FFFF0000"}'
```

## Example of Python API

```sh
curl --data-binary '@./Demo/Scripts/clickComponent.py' http://localhost:8001 -H 'Content-Type: text/x-python'
curl --data-binary '@./Demo/Scripts/findComponent.py' http://localhost:8001 -H 'Content-Type: text/x-python'
curl --data-binary '@./Demo/Scripts/log.py' http://localhost:8001 -H 'Content-Type: text/x-python'
curl --data-binary '@./Demo/Scripts/raise.py' http://localhost:8001 -H 'Content-Type: text/x-python'
curl --data-binary '@./Demo/Scripts/test.py' http://localhost:8001 -H 'Content-Type: text/x-python'
```
