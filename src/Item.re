/* State declaration */
type item_state = Todo | Done;

type item = {
  id: string,
  name: string,
  item_state: item_state,
};

type state = {
  item: item,
  itemRef: ref(option(Dom.element))
};

type action =
  | Edit(string)
  | ToggleState;

let component = ReasonReact.reducerComponent("Item");

let new_item = (name) => {
  id: [%bs.raw {| String(Math.random()) |}],
  name: name,
  item_state: Todo
};

let demo_items = () => [|
  {
  id: [%bs.raw {| String(Math.random()) |}],
  name: "Item 1",
  item_state: Todo
  },
                        {
  id: [%bs.raw {| String(Math.random()) |}],
  name: "Item 2",
  item_state: Todo
}
|];

let toggle_state = (state) =>
  switch(state) {
  | Todo => Done
  | Done => Todo
  };

let setItemRef = (theRef, {ReasonReact.state}) => {
  state.itemRef := Js.Nullable.toOption(theRef);
};

let make = (~it, _children) => {
  ...component,

  initialState: () => {
    item: it,
    itemRef: ref(None)
  },

  /* TODO: How to store (in ReasonML JSX) dataId=it.id ??? */
  didMount: self => {
    let js_set_data = [%bs.raw {|
       function(el, iid) {
         $(el).data('id', iid);
       }
    |}];
    js_set_data(self.state.itemRef, self.state.item.id);
  },

  reducer: (action, state) =>
    switch(action) {
      | Edit(text)  => ReasonReact.Update({...state, item: {...state.item, name: text} })
      | ToggleState => ReasonReact.Update({
        ...state,
        item: {
          ...state.item,
          item_state: toggle_state(state.item.item_state)
        }
      })
    },

  render: (self) => {
    let stateName = (s) =>
      switch(s) {
      | Todo => "TODO"
      | Done => "DONE"
      };

    let doneIcon = (item) =>
      switch(item.item_state) {
      | Todo => <span />
      | Done => (
          <span className="icon is-small is-right">
            <i className="fas fa-check has-text-success"></i>
          </span>
      )
      };

    <div className="columns"
         ref={self.handle(setItemRef)}>
      <div className="control column has-icons-right">
        <div className="button is-fullwidth"
            onClick=(_event => self.send(ToggleState))>
        <span className="name is-left">(ReasonReact.string(self.state.item.name))</span>
        (doneIcon(self.state.item))
      </div>
    </div>
    <div className="column is-one-fifth state"
        onClick=(_event => self.send(ToggleState))>(ReasonReact.string(stateName(self.state.item.item_state)))</div>
  </div>
  }
};
