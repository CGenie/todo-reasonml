# `todo-reasonml`

[TodoMVC](http://todomvc.com/) is a nice project for comparing JavaScript frameworks. However, I find it too simple -- it doesn't cover more advanced requirements which are put on the developer in the real world.

One of good tests turns out to be an app with dynamically rendered lists, where you can drag elements between the lists (so a mult-list TodoMVC).

Turns out in this setting the frameworks don't become that easy anymore. One example is AngularJS 1.x. Just read [this thread](https://github.com/angular/angular.js/pull/4786).

## Run Project

```sh
npm install
npm start
# in another tab
npm run webpack
```

After you see the webpack compilation succeed (the `npm run webpack` step), open up `src/index.html` (**no server needed!**). Then modify whichever `.re` file in `src` and refresh the page to see the changes.

**For more elaborate ReasonReact examples**, please see https://github.com/reasonml-community/reason-react-example

## Build for Production

```sh
npm run build
npm run webpack:production
```

This will replace the development artifact `build/Index.js` for an optimized version.

**To enable dead code elimination**, change `bsconfig.json`'s `package-specs` `module` from `"commonjs"` to `"es6"`. Then re-run the above 2 commands. This will allow Webpack to remove unused code.
