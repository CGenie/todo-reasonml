type todo_list = {
  id: string,
  name: string,
  items: array(Item.item),
};

type state = {
  todo_list: todo_list,
  new_item_name: string,
  listRef: ref(option(Dom.element)),
  sortableRef: ref(option(Dom.element))
};

type action =
  | Edit(string)
  | AddItem(string)
  | ChangeNewItemName(string);

let component = ReasonReact.reducerComponent("TodoList");

let empty_list = (name) => {
id: [%bs.raw {| String(Math.random()) |}],
  name: name,
  items: [||]
};

let demo_list = (num) => {
  id: [%bs.raw {| String(Math.random()) |}],
  name: "List " ++ string_of_int(num),
  items: Item.demo_items()
};

let rec demo_lists = (num) =>
  switch(num) {
    | 0 => [| |]
    | n => Array.append([| demo_list(num) |], demo_lists(num - 1))
  };

let add_item = (tl: todo_list, n: string) => {
  ...tl,
  items: Array.append(
    tl.items,
    [| Item.new_item(n) |]
  )
};

let move_item = (tls: array(todo_list), src_list_id: string, src_item_id: string, dest_list_id: string) => {
  Js.log(("tls", tls));
  let src_lst: todo_list = List.find((lst) => lst.id == src_list_id, Array.to_list(tls));
  Js.log(("src_lst", src_lst));
  let src_item: Item.item = List.find((it: Item.item) => it.id == src_item_id, Array.to_list(src_lst.items));
  Js.log(("src_item", src_item));
  let dest_lst: todo_list = List.find((lst) => lst.id == dest_list_id, Array.to_list(tls));
  Js.log(("dest_lst", dest_lst));

  /* TODO: dest item position is needed */

  Array.map(
    (lst: todo_list) => {
      if (lst.id == src_list_id) {
        let fltr = List.filter ((it: Item.item) => it.id != src_item_id);
        let filtered = lst.items -> Array.to_list -> fltr -> Array.of_list;
        {...lst, items: filtered}
      } else if (lst.id == dest_list_id) {
        {...lst, items: Array.append(lst.items, [| src_item |])}
      } else lst
    }, tls
  )
};

let setListRef = (theRef, {ReasonReact.state}) => {
  state.listRef := Js.Nullable.toOption(theRef);
};

let setSortableRef = (theRef, {ReasonReact.state}) => {
  state.sortableRef := Js.Nullable.toOption(theRef);
};

let make = (
    ~lst,
    ~onDragItem,
    _children) => {
  ...component,

  initialState: () => {
    todo_list: lst,
    new_item_name: "",
    listRef: ref(None),
    sortableRef: ref(None)
  },

  didMount: self => {
    /* Set list-id in data */
    let js_set_data = [%bs.raw {|
      function(el, iid) {
        $(el).data('list-id', iid);
      }
    |}];
    js_set_data(self.state.listRef, self.state.todo_list.id);

    /* Set up the sortable */
    let sortable_on_update = (event, ui) => {
      Js.log(("event", event));
      Js.log(("ui", ui));
      let js_get_src_item_id = [%bs.raw {|
           function (ui) {
           return $(ui.item).data('id');
           }
           |}];
      let js_get_src_list_id = [%bs.raw {|
        function (ui) {
          if(ui.sender) {
            return $(ui.sender).closest('.list').data('list-id');
          }
        }
      |}];
      let src_item_id : string = js_get_src_item_id(ui);
      let opt_src_list_id : option(string) = js_get_src_list_id(ui);
      let src_list_id = switch(opt_src_list_id) {
        | None => self.state.todo_list.id
        | Some(iid) => iid
      };
      Js.log(("src_item_id", src_item_id));
      Js.log(("src_list_id", src_list_id));
      Js.log(("self.state.todo_list.id", self.state.todo_list.id));
      onDragItem(src_list_id, src_item_id, self.state.todo_list.id);
    };

    let js_set_sortable = [%bs.raw {|
      function (el, onUpdate) {
        $(el).sortable({
          connectWith: '.js-sortable',
          receive: onUpdate,
          over: function() { $('.placeholder').hide() },
          out: function() { $('.placeholder').show() },
          stop: function() { $('.placeholder').hide() },
        });
        return null;
      }
    |}];
    js_set_sortable(self.state.sortableRef, sortable_on_update);
  },

  reducer: (action, state) =>
    switch(action) {
    | Edit(text) => ReasonReact.Update({...state, todo_list: {...state.todo_list, name: text} })
    | AddItem(n) => ReasonReact.Update({
        ...state,
        todo_list: add_item(state.todo_list, n),
        new_item_name: ""
      })
    | ChangeNewItemName(text) => ReasonReact.Update({...state, new_item_name: text})
    },

  render: self => {
    let itemsList = (items) =>
      Array.mapi
      (
        (index, it :Item.item) => {
          <Item key=it.id it />
        },
        items
      );
    <div className="level-item list"
         ref={self.handle(setListRef)}>
      <div className="card">
        <header className="card-header">
          <p className="card-header-title">(ReasonReact.string(self.state.todo_list.name))</p>
        </header>
        <div className="card-content">
          <div className="content">
            <div className="items js-sortable"
                ref={self.handle(setSortableRef)}>
              /* Dummy item to allow easy drop to an empty list. */
              /* Hides and shows with over/out/stop events. */
              /* See: https://stackoverflow.com/questions/11036724/display-placeholder-text-in-empty-sortable-with-jquery-ui */
              <div className="columns placeholder" style=(ReactDOMRe.Style.make(~display="none", ()))><div className="column">(ReasonReact.string(" "))</div></div>
              (ReasonReact.array(itemsList(self.state.todo_list.items)))
            </div>
          </div>
        </div>
        <footer className="card-footer">
          <div className="field">
            <div className="control">
              <input className="input is-primary"
                  value=self.state.new_item_name
                  onChange=(
                    event=>
                        self.send(ChangeNewItemName(ReactEvent.Form.target(event)##value))
                  )/>
              <a className="button" onClick=(_event => self.send(AddItem(self.state.new_item_name)))>
                  (ReasonReact.string("Add item"))
              </a>
            </div>
          </div>
        </footer>
      </div>
    </div>
  }
};
