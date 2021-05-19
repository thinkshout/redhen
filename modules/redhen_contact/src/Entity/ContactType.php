<?php

namespace Drupal\redhen_contact\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;
use Drupal\redhen_contact\ContactTypeInterface;

/**
 * Defines the Contact type entity.
 *
 * @ConfigEntityType(
 *   id = "redhen_contact_type",
 *   label = @Translation("Contact type"),
 *   handlers = {
 *     "list_builder" = "Drupal\redhen_contact\ContactTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\redhen_contact\Form\ContactTypeForm",
 *       "edit" = "Drupal\redhen_contact\Form\ContactTypeForm",
 *       "delete" = "Drupal\redhen_contact\Form\ContactTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_contact\ContactTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "redhen_contact_type",
 *   admin_permission = "administer site configuration",
 *   bundle_of = "redhen_contact",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/redhen/contact_type/{redhen_contact_type}",
 *     "add-form" = "/admin/structure/redhen/contact_type/add",
 *     "edit-form" = "/admin/structure/redhen/contact_type/{redhen_contact_type}/edit",
 *     "delete-form" = "/admin/structure/redhen/contact_type/{redhen_contact_type}/delete",
 *     "collection" = "/admin/structure/redhen/contact_type"
 *   },
 *   config_export = {
 *     "uuid",
 *     "status",
 *     "id",
 *     "label",
 *   }
 * )
 */
class ContactType extends ConfigEntityBundleBase implements ContactTypeInterface {
  /**
   * The Contact type ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Contact type label.
   *
   * @var string
   */
  protected $label;

}
