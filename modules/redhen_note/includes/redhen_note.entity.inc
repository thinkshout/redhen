<?php
/**
 * @file
 * Redhen Group entity classes
 */

/**
 * The class used for note entities.
 */
class RedhenNote extends Entity {

  public
    $note_id = NULL,
    $author_uid = NULL,
    $author = NULL,
    $type = '',
    $entity = NULL,
    $created = '',
    $updated = '';

  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_note');

    // New note. is_new might not be set so check for id
    if ($this->note_id) {
      $this->entity = $this->entity();
      $this->author = $this->author();
    }
    else {
      global $user;
      $this->created = REQUEST_TIME;
      $this->author_uid = $user->uid;
    }
  }

  /**
   * Override buildContent() to add note properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {

    $content['author'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Author'),
      '#items' => array(
        array(
          '#markup' => $this->author->name,
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    $content['created'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Created'),
      '#items' => array(
        array(
          '#markup' => format_date($this->created),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    return entity_get_controller($this->entityType)
      ->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Override label() to add note label.
   */
  public function label() {
    return "Note {$this->note_id} on {$this->entity->label()}";
  }

  /**
   * Permanently saves the entity.
   *
   * @return bool
   *  Returns FALSE if entity was not saved.
   */
  public function save() {
    $this->updated = REQUEST_TIME;
    return parent::save();
  }

  /*
   * Get entity note is attached to
   */
  public function entity() {
    if (isset($this->entity_type)) {
      $entity = entity_load($this->entity_type, array($this->entity_id));
      return $entity[$this->entity_id];
    }
    else {
      return NULL;
    }
  }

  /*
   * Get author of note
   */
  public function author() {
    $user = user_load($this->author_uid);
    return $user;
  }

  /**
   * Specifify URI
   */
  protected function defaultUri() {
    switch ($this->entity_type) {
      case 'redhen_contact':
        $path = 'contact';
        break;
      case 'redhen_org':
        $path = 'org';
        break;
    }
    return array('path' => 'redhen/' . $path . '/' . $this->entity_id . '/note/' . $this->note_id . '/view');
  }


}

/**
 * Controls metadata for Redhen notes
 *
 * Redhen utilizes custom property attributes to determine if a property
 * should be available as a filter on the listing page. These attributes are
 *   filter: TRUE to add as a filter.
 *   field_type: textfield, select, etc.
 *   options: array of options for select field types.
 */
class RedhenNoteMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['created'] = array(
      'label' => t("Created"),
      'description' => t("The date the note was created."),
      'type' => 'date',
      'filter' => TRUE,
      'field_type' => 'textfield',
      'schema field' => 'created',
    );
    $properties['updated'] = array(
      'label' => t("Updated"),
      'description' => t("The date the note was updated."),
      'type' => 'date',
      'schema field' => 'updated',
    );
    $properties['author'] = array(
      'label' => t("Author"),
      'type' => 'user',
      'description' => t("The author of the note."),
      'schema field' => 'author_uid'
    );

    return $info;
  }

}

